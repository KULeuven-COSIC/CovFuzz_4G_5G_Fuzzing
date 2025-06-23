cmake_minimum_required(VERSION 3.14.5)

execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/VERSION
    OUTPUT_VARIABLE Version
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

project(prometheus-client-c VERSION ${Version} LANGUAGES C)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
  if(EXISTS "/etc/debian_version")
    if(CMAKE_LIBRARY_ARCHITECTURE)
      set(CMAKE_INSTALL_LIBDIR "lib/${CMAKE_LIBRARY_ARCHITECTURE}")
    endif()
  endif()
endif()

add_subdirectory (prom)

# Sometimes linking against libatomic is required for atomic ops, if
# the platform doesn't support lock-free atomics.

include(CheckCSourceCompiles)
include(CheckLibraryExists)

function(check_woring_c_atomics varname)
  check_c_source_compiles("
#if HAVE_STDINT_H
#include <stdint.h>
#endif
int main(int argc, const char *const *argv)
{
#if HAVE_STDINT_H
    uint32_t val = 1010, tmp, *mem = &val, *ptmp;
#else
    unsigned int val = 1010, tmp, *mem = &val, *ptmp;
#endif

    if (__atomic_fetch_add(&val, 1010, __ATOMIC_SEQ_CST) != 1010 || val != 2020)
        return 1;

    tmp = val;
    if (__atomic_fetch_sub(mem, 1010, __ATOMIC_SEQ_CST) != tmp || val != 1010)
        return 1;

    if (__atomic_sub_fetch(&val, 1010, __ATOMIC_SEQ_CST) != 0 || val != 0)
        return 1;

    tmp = val;
    if (!__atomic_compare_exchange_n(mem, &tmp, 3030, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
            || tmp != 0)
        return 1;

    if (__atomic_exchange_n(&val, 4040, __ATOMIC_SEQ_CST) != 3030)
        return 1;

    ptmp = &val;
    if (!__atomic_compare_exchange_n(&mem, &ptmp, &tmp, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
            || ptmp != &val || mem != &tmp)
        return 1;

    return 0;
}
" ${varname})
endfunction(check_woring_c_atomics)

function(check_woring_c_atomics64 varname)
  check_c_source_compiles("
#if HAVE_STDINT_H
#include <stdint.h>
#endif
int main(int argc, const char *const *argv)
{
#if HAVE_STDINT_H
    uint64_t val = 1010, tmp, *mem = &val;
#else
    unsigned long long val = 1010, tmp, *mem = &val;
#endif

    if (__atomic_fetch_add(&val, 1010, __ATOMIC_SEQ_CST) != 1010 || val != 2020)
        return 1;

    tmp = val;
    if (__atomic_fetch_sub(mem, 1010, __ATOMIC_SEQ_CST) != tmp || val != 1010)
        return 1;

    if (__atomic_sub_fetch(&val, 1010, __ATOMIC_SEQ_CST) != 0 || val != 0)
        return 1;

    tmp = val;
    if (!__atomic_compare_exchange_n(mem, &tmp, 3030, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
            || tmp != 0)
        return 1;

    if (__atomic_exchange_n(&val, 4040, __ATOMIC_SEQ_CST) != 3030)
        return 1;

    return 0;
}
" ${varname})
endfunction(check_woring_c_atomics64)

# Check for (non-64-bit) atomic operations.
if(MSVC)
  set(HAVE_C_ATOMICS_WITHOUT_LIB True)
else()
  # First check if atomics work without the library.
  check_woring_c_atomics(HAVE_C_ATOMICS_WITHOUT_LIB)
  # If not, check if the library exists, and atomics work with it.
  if(NOT HAVE_C_ATOMICS_WITHOUT_LIB)
    check_library_exists(atomic __atomic_fetch_add_4 "" HAVE_LIBATOMIC)
    if(HAVE_LIBATOMIC)
      target_link_libraries(prom PUBLIC atomic)
    else()
      message(FATAL_ERROR "Host compiler appears to require libatomic, but cannot find it.")
    endif()
  endif()
endif()

# Check for 64 bit atomic operations.
if(MSVC)
  set(HAVE_C_ATOMICS64_WITHOUT_LIB True)
else()
  # First check if atomics work without the library.
  check_woring_c_atomics64(HAVE_C_ATOMICS64_WITHOUT_LIB)
  # If not, check if the library exists, and atomics work with it.
  if(NOT HAVE_C_ATOMICS64_WITHOUT_LIB)
    check_library_exists(atomic __atomic_load_8 "" HAVE_C_LIBATOMICS64)
    if(HAVE_C_LIBATOMICS64)
      target_link_libraries(prom PUBLIC atomic)
    else()
      message(FATAL_ERROR "Host compiler appears to require libatomic for 64-bit operations, but cannot find it.")
    endif()
  endif()
endif()
