idf_component_register(SRCS "asm_main.c"
                    INCLUDE_DIRS ""
                    REQUIRES soc nvs_flash ulp log)
#
# ULP support additions to component CMakeLists.txt.
#
# 1. The ULP app name must be unique (if multiple components use ULP).
set(asm_app_name asm_${COMPONENT_NAME})
#
# 2. Specify all assembly source files.
#    Files should be placed into a separate directory (in this case, ulp/),
#    which should not be added to COMPONENT_SRCS.
set(asm_sources "asm/asm.S")
#
# 3. List all the component source files which include automatically
#    generated ULP export file, ${ulp_app_name}.h:
set(asm_dep_srcs "asm_main.c")
#
# 4. Call function to build ULP binary and embed in project using the argument
#    values above.
ulp_embed_binary(${asm_app_name} "${asm_sources}" "${asm_dep_srcs}")
