#include "hal_data.h"
#include "yolo/yolo_api.h"
#include "zf_common_headfile.h"
#include "zf_device_ips200.h"
#include <stdbool.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>

#if (1 == BSP_MULTICORE_PROJECT) && BSP_TZ_SECURE_BUILD
bsp_ipc_semaphore_handle_t g_core_start_semaphore = { .semaphore_num = 0 };
#endif

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/* GCC 编译桩函数 */
int _close(int file) { (void)file; return -1; }
int _lseek(int file, int ptr, int dir) { (void)file; (void)ptr; (void)dir; return 0; }
int _read(int file, char *ptr, int len) { (void)file; (void)ptr; (void)len; return 0; }
int _fstat(int file, struct stat *st) { (void)file; if (st) { st->st_mode = S_IFCHR; } return 0; }
int _isatty(int file) { (void)file; return 1; }

void g_ipc1_callback(ipc_callback_args_t *p_args) { (void)p_args; }

// ================= 大核记忆坐标 =================
// 用于在没有识别到蓝莓时，依然能发出最后一次的坐标
static int16_t g_last_x = 0;
static int16_t g_last_y = 0;
static int16_t g_last_z = 0;
// ==============================================

/* 【修复】：增加微小延时，防止 IPC 硬件邮箱拥堵导致后续坐标丢失 */
static bool send_ipc_data(int16_t x, int16_t y, int16_t z, uint8_t status)
{
    // 强制截取后 16 位，完美支持负数坐标发送
    uint32_t msg_x = (0x11 << 24) | ((uint32_t)(uint16_t)x);
    uint32_t msg_y = (0x12 << 24) | ((uint32_t)(uint16_t)y);
    uint32_t msg_z = (0x13 << 24) | ((uint32_t)(uint16_t)z);
    uint32_t msg_s = (0x14 << 24) | ((uint32_t)(uint16_t)status);

    fsp_err_t err_x, err_y, err_z, err_s;

    // 1. 发送 X 坐标，等待 1 毫秒让小核取件
    err_x = R_IPC_MessageSend(&g_ipc0_ctrl, msg_x);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // 2. 发送 Y 坐标
    err_y = R_IPC_MessageSend(&g_ipc0_ctrl, msg_y);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // 3. 发送 Z 坐标
    err_z = R_IPC_MessageSend(&g_ipc0_ctrl, msg_z);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    // 4. 发送 Status 标志位
    err_s = R_IPC_MessageSend(&g_ipc0_ctrl, msg_s);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

    return (err_x == FSP_SUCCESS && err_y == FSP_SUCCESS && err_z == FSP_SUCCESS && err_s == FSP_SUCCESS);
}

typedef struct {
    float fx, fy, cx, cy, real_diameter_mm;
} camera_calib_t;

static camera_calib_t g_camera_calib = {
    .fx = 133.46f, .fy = 171.00f,
    .cx = 76.50f,  .cy = 74.34f,
    .real_diameter_mm = 18.85f  /* 蓝莓实际大小保持不变 */
};

typedef struct {
    int16_t x_mm, y_mm, z_mm;
} ai_target_3d_t;

#ifndef RGB_BUF_SIZE
#define RGB_BUF_SIZE (SCC8660_W * SCC8660_H * 3)
#endif

#ifndef handle_error
#define handle_error(err, msg) do { \
    if (FSP_SUCCESS != (err)) { \
        printf("[ERROR] %s\r\n", (msg)); \
        while(1); \
    } \
} while(0)
#endif

extern void yuyv_to_rgb888(const uint8_t *src, int width, int height, uint8_t *dst, int flip);
extern void geo_correct_rgb888(const uint8_t *src, uint8_t *dst, int width, int height, float k1, float k2, float aspect);

static uint8_t g_rgb_buffer[RGB_BUF_SIZE] BSP_ALIGN_VARIABLE(64);
static uint8_t g_rgb_corrected[RGB_BUF_SIZE] BSP_ALIGN_VARIABLE(64);
static uint16_t g_rgb565_buffer[SCC8660_W * SCC8660_H] BSP_ALIGN_VARIABLE(64);

#define GEO_K1     -0.15f
#define GEO_K2      0.00f
#define GEO_ASPECT  1.10f

static void rgb888_to_rgb565_buffer(const uint8_t *src, uint16_t *dst, int pixels)
{
    for (int i = 0; i < pixels; i++) {
        uint8_t r = src[i * 3];
        uint8_t g = src[i * 3 + 1];
        uint8_t b = src[i * 3 + 2];
        dst[i] = (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
    }
}

void hal_entry(void)
{
    R_IPC_Open(&g_ipc0_ctrl, &g_ipc0_cfg);
    R_IPC_Open(&g_ipc1_ctrl, &g_ipc1_cfg);

#if (0 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT) && !BSP_TZ_NONSECURE_BUILD
#if BSP_TZ_SECURE_BUILD
    R_BSP_IpcSemaphoreTake(&g_core_start_semaphore);
#endif
    R_BSP_SecondaryCoreStart();
#if BSP_TZ_SECURE_BUILD
    while(FSP_ERR_IN_USE == R_BSP_IpcSemaphoreTake(&g_core_start_semaphore)) {;}
#endif
#endif

    fsp_err_t err = FSP_SUCCESS;
    bsp_io_level_t readLevel;

    debug_init();
    printf("\r\n[SYS] Hal entry started\r\n");

    err = g_ioport.p_api->open(g_ioport.p_ctrl, g_ioport.p_cfg);
    handle_error(err, "IOPORT ERR");

    R_BSP_SdramInit(true);

    err = g_rm_ethosu0.p_api->open(g_rm_ethosu0.p_ctrl, g_rm_ethosu0.p_cfg);
    handle_error(err, "NPU ERR");
    static YoloApi yolo_api;
    YoloApi_Init(&yolo_api);
    printf("[SYS] YOLO Initialized\r\n");

    while (1) {
        if (!scc8660_init()) { break; }
        R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);
    }

    ips200_set_dir(IPS200_CROSSWISE);
    ips200_init();
    ips200_set_font(IPS200_8X16_FONT);
    ips200_set_color(RGB565_BLACK, RGB565_WHITE);
    ips200_full(RGB565_WHITE);
    ips200_show_string(92, 112, "AI Ready");
    printf("[SYS] IPS200 Init Success\r\n");

    uint32_t frame_count = 0U;
    ai_target_3d_t targets_3d[10];

    while (true) {
//        if (!scc8660_finish_flag) {
//            __WFI();
//            continue;
//        }
//        scc8660_finish_flag = false;
        frame_count++;
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
        yuyv_to_rgb888((const uint8_t *)scc8660_image, SCC8660_W, SCC8660_H, g_rgb_buffer, 1);
        geo_correct_rgb888(g_rgb_buffer, g_rgb_corrected, SCC8660_W, SCC8660_H, GEO_K1, GEO_K2, GEO_ASPECT);

        rgb888_to_rgb565_buffer(g_rgb_corrected, g_rgb565_buffer, SCC8660_W * SCC8660_H);
        ips200_show_rgb565_image(0, 0, g_rgb565_buffer, SCC8660_W, SCC8660_H, 240, 240, 0);

        ips200_set_color(RGB565_RED, RGB565_BLACK);
        ips200_draw_line(0, 120, 239, 120, RGB565_RED);
        ips200_show_string(230, 104, "X");
        ips200_draw_line(120, 0, 120, 239, RGB565_RED);
        ips200_show_string(124, 224, "Y");

        YoloDetectionResult *det_result = YoloApi_RunYolo(&yolo_api, g_rgb_corrected);
        YoloApi_FilterByWidth(det_result, 10.0f);

        ips200_set_color(RGB565_GREEN, RGB565_BLACK);
        uint8_t valid_count = 0;
        int32_t min_xy_dist_sq = 2147483647;
        int8_t best_target_idx = -1;

        for (int i = 0; i < det_result->count; i++) {
            YoloDetection *d = &det_result->detections[i];
            float real_cx = d->x;
            float real_cy = d->y;
            float real_w  = d->w;
            float real_h  = d->h;

            float pixel_diameter = (real_w > real_h) ? real_w : real_h;
            if (pixel_diameter < 1.0f) pixel_diameter = 1.0f;

            float Z_mm = (g_camera_calib.real_diameter_mm * g_camera_calib.fx) / pixel_diameter;
            float X_mm = (real_cx - g_camera_calib.cx) * Z_mm / g_camera_calib.fx;
            float Y_mm = (real_cy - g_camera_calib.cy) * Z_mm / g_camera_calib.fy;

            if (valid_count < 10) {
                targets_3d[valid_count].x_mm = (int16_t)X_mm;
                targets_3d[valid_count].y_mm = (int16_t)Y_mm;
                targets_3d[valid_count].z_mm = (int16_t)Z_mm;

                int32_t current_dist_sq = ((int32_t)X_mm * (int32_t)X_mm) + ((int32_t)Y_mm * (int32_t)Y_mm);
                if (current_dist_sq < min_xy_dist_sq) {
                    min_xy_dist_sq = current_dist_sq;
                    best_target_idx = valid_count;
                }
                valid_count++;
            }

            int x1 = (int)((d->x - d->w / 2.0f) * 1.5f);
            int y1 = (int)((d->y - d->h / 2.0f) * 1.5f);
            int x2 = (int)((d->x + d->w / 2.0f) * 1.5f);
            int y2 = (int)((d->y + d->h / 2.0f) * 1.5f);

            x1 = (x1 < 1) ? 1 : ((x1 > 238) ? 238 : x1);
            y1 = (y1 < 1) ? 1 : ((y1 > 238) ? 238 : y1);
            x2 = (x2 < 1) ? 1 : ((x2 > 238) ? 238 : x2);
            y2 = (y2 < 1) ? 1 : ((y2 > 238) ? 238 : y2);

            if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
            if (y1 > y2) { int t = y1; y1 = y2; y2 = t; }

            if (x1 != x2 && y1 != y2) {
                ips200_draw_line((uint16_t)x1, (uint16_t)y1, (uint16_t)x2, (uint16_t)y1, RGB565_GREEN);
                ips200_draw_line((uint16_t)x2, (uint16_t)y1, (uint16_t)x2, (uint16_t)y2, RGB565_GREEN);
                ips200_draw_line((uint16_t)x2, (uint16_t)y2, (uint16_t)x1, (uint16_t)y2, RGB565_GREEN);
                ips200_draw_line((uint16_t)x1, (uint16_t)y2, (uint16_t)x1, (uint16_t)y1, RGB565_GREEN);
            }
        }

        if (best_target_idx > 0) {
            ai_target_3d_t temp = targets_3d[0];
            targets_3d[0] = targets_3d[best_target_idx];
            targets_3d[best_target_idx] = temp;
        }

        /* ================= 核心发送逻辑 ================= */
        if (valid_count > 0) {
            // 更新记忆坐标
            g_last_x = targets_3d[0].x_mm;
            g_last_y = targets_3d[0].y_mm;
            g_last_z = targets_3d[0].z_mm;

            // 发送实时坐标，且 Status 置为 1 (识别有效)
            bool ok = send_ipc_data(g_last_x, g_last_y, g_last_z, 1);
            if (frame_count % 3 == 0) {
                printf("[AI] Count: %d | X: %d Y: %d Z: %d | Status: 1 | TX: %s\r\n",
                       valid_count, g_last_x, g_last_y, g_last_z, ok ? "OK" : "FAIL");
            }
        } else {
            // 发送记忆坐标，且 Status 置为 0 (丢失目标/使用旧数据)
            bool ok = send_ipc_data(g_last_x, g_last_y, g_last_z, 0);
            if (frame_count % 3 == 0) {
                printf("[AI] Count: 0 | X:%d Y:%d Z:%d | Status: 0 | TX: %s\r\n",
                       g_last_x, g_last_y, g_last_z, ok ? "OK" : "FAIL");
            }
        }

        bsp_io_level_t readLevel;
        g_ioport.p_api->pinRead(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_10, &readLevel);
        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_10, BSP_IO_LEVEL_HIGH - readLevel);

        if (frame_count % 3 == 0) {
            ips200_set_color(RGB565_WHITE, RGB565_BLACK);
            ips200_show_string(242, 0,   "Detect:");
            ips200_show_uint(242, 16, valid_count, 2);

            ips200_show_string(242, 36, "      ");
            ips200_show_string(242, 52, "      ");
            ips200_show_string(242, 72, "      ");
            ips200_show_string(242, 88, "      ");

            if (valid_count > 0) {
                ips200_show_string(242, 36, "Score:");
                ips200_show_float(242, 52, det_result->detections[0].score, 1, 3);
                ips200_show_string(242, 72, "Z(mm):");
                ips200_show_float(242, 88, (double)targets_3d[0].z_mm, 3, 1);
            }
        }
    }

    YoloApi_Deinit(&yolo_api);
}
