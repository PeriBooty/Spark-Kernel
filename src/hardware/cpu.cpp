#include <hardware/cpu.hpp>

void CentralProcessingUnit::cpuid(int code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    asm volatile("cpuid"
                 : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                 : "a"(code));
}

extern "C" int cpuidavailable();
// TODO: Rewrite this shit code
cpu CentralProcessingUnit::get_cpu() {
    cpu cpu{};
    cpu.vendor = "Unknown CPU";
    cpu.features = {};
    if (cpuidavailable() == 0)
        return cpu;
    uint32_t magic_code, unused;
    cpuid(0, &unused, &magic_code, &unused, &unused);
    switch (magic_code) {
        case 0x756e6547: {
            uint32_t eax, ebx, /*ecx,*/ /*edx,*/ /*max_eax,*/ signature, unused;
            int /*model,*/ /*family,*/ /*type,*/ brand /*, stepping,*/ /*reserved*/;
            //int extended_family = -1;
            cpuid(1, &eax, &ebx, &unused, &unused);
            //model = (eax >> 4) & 0xf;
            //family = (eax >> 8) & 0xf;
            //type = (eax >> 12) & 0x3;
            brand = ebx & 0xff;
            //stepping = eax & 0xf;
            //reserved = eax >> 14;
            signature = eax;
            cpu.vendor = signature == 0x000006B1 || signature == 0x00000F13 ? intel_other[brand] : intel[brand];
            break;
        }
        case 0x68747541: {
            cpu.vendor = "AMD";
            break;
        }
    }
    return cpu;
}