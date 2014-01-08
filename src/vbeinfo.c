#include "vbe.h"

#include <stdio.h>
#include <stdlib.h>

static void print_vbe_info(FILE *file, struct vbe_info *info)
{
    fprintf(file,
        "\nVBE controller information\n"
        "----------------------------------------------------------------\n"
        "VBE signature:                 '%4s'\n"
        "VBE version:                   %Xh\n"
        "OEM string:                    %s\n"
        "Capabilities:                  %Xh\n"
        "Total memory:                  %d KB\n"
        "OEM software revision:         %Xh\n"
        "OEM vendor name:               %s\n"
        "OEM product name:              %s\n"
        "OEM product revision:          %s\n\n",
        (char *) &info->vbe_signature,
        info->vbe_version,
        info->oem_string,
        info->capabilities,
        info->total_memory << 6,
        info->oem_software_rev,
        info->oem_vendor_name,
        info->oem_product_name,
        info->oem_product_rev);

    fprintf(file, "Supported video modes:\n\n");

    for (uint16_t *mode = info->video_modes; *mode != 0xFFFF; mode++)
        fprintf(file, "%Xh ", *mode);

    fprintf(file, "\n");
}

static void print_vbe_mode_attributes(FILE *file, unsigned int attr)
{
    fprintf(file,
        "Mode supported by hardware:    %s\n"
        "BIOS TTY output functions:     %s\n"
        "Color mode:                    %s\n"
        "Graphics mode:                 %s\n"
        "VGA compatible:                %s\n"
        "Windowed framebuffer:          %s\n"
        "Linear framabuffer:            %s\n"
        "Double scan mode:              %s\n"
        "Interlaced mode:               %s\n"
        "Triple buffering:              %s\n"
        "Stereo display:                %s\n"
        "Dual display start address:    %s\n",
        attr & VBE_MODE_SUPPORTED_BY_HARDWARE ?
            "yes" : "no",
        attr & VBE_BIOS_TTY_OUTPUT_FUNCTIONS_SUPPORTED ?
            "supported" : "not supported",
        attr & VBE_COLOR_MODE ?
            "yes" : "no",
        attr & VBE_GRAPHICS_MODE ?
            "yes" : "no",
        attr & VBE_MODE_NOT_VGA_COMPATIBLE ?
            "no" : "yes",
        attr & VBE_WINDOWED_FRAMEBUFFER_NOT_SUPPORTED ?
            "not supported" : "supported",
        attr & VBE_LINEAR_FRAMEBUFFER_SUPPORTED ?
            "supported" : "not supoorted",
        attr & VBE_DOUBLE_SCAN_MODE_SUPPORTED ?
            "supported" : "not supported",
        attr & VBE_INTERLACED_MODE_SUPPORTED ?
            "supported" : "not supported",
        attr & VBE_TRIPLE_BUFFERING_SUPPORTED ?
            "supported" : "not supported",
        attr & VBE_STEREO_DISPLAY_SUPPORTED ?
            "supported" : "not supported",
        attr & VBE_DUAL_DISPLAY_START_ADDRESS_SUPPORTED ?
            "supported" : "not supported");
}

static void print_vbe_mode_info(FILE *file, int mode, struct vbe_mode_info *info)
{
    fprintf(file,
        "\nVBE mode information: %Xh\n"
        "----------------------------------------------------------------\n"
        "Window granularity:            %d KB\n"
        "Window size:                   %d KB\n"
        "Bytes per scanline:            %d\n"
        "X resolution:                  %d\n"
        "Y resolution:                  %d\n"
        "Number of planes:              %d\n"
        "Bits per pixel:                %d\n"
        "Number of banks:               %d\n"
        "Bank size:                     %d\n"
        "Number of image pages:         %d\n",
        mode,
        info->win_granularity,
        info->win_size,
        info->bytes_per_scanline,
        info->x_resolution,
        info->y_resolution,
        info->number_of_planes,
        info->bits_per_pixel,
        info->number_of_banks,
        info->bank_size,
        info->number_of_image_pages);

    print_vbe_mode_attributes(file, info->mode_attributes);
}

int main(int argc, char *argv[])
{
    struct vbe_info info;
    struct vbe_mode_info mode_info;

    FILE *file = fopen("vbeinfo.txt", "w");

    if (!file)
        goto failure;

    if (vbe_get_info(&info) != 0)
        goto failure;

    print_vbe_info(file, &info);

    for (uint16_t *mode = info.video_modes; *mode != 0xFFFF; mode++) {
        if (vbe_get_mode_info(*mode, &mode_info) != 0)
            goto failure;
        print_vbe_mode_info(file, *mode, &mode_info);
    }

    fclose(file);
    exit(EXIT_SUCCESS);

failure:

    if (file)
        fclose(file);

    exit(EXIT_FAILURE);
}
