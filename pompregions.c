

#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>

extern void POMP2_Init_reg_7471350_1();

void POMP2_Init_regions()
{
    POMP2_Init_reg_7471350_1();
}

size_t POMP2_Get_num_regions()
{
    return 1;
}

const char* POMP2_Get_opari2_version()
{
    return "1.0.6";
}

/* 
 * The following functions define the POMP2 library interface version
 * the instrumented code conforms with. The library interface version
 * is modeled after
 * https://www.gnu.org/software/libtool/manual/libtool.html#Versioning
 */

int POMP2_Get_required_pomp2_library_version_current()
{
    return 1;
}

int POMP2_Get_required_pomp2_library_version_revision()
{
    return 0;
}

int POMP2_Get_required_pomp2_library_version_age()
{
    return 0;
}

#ifdef __cplusplus
}
#endif
