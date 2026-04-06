if %1.==. goto conversion
cd %1

:conversion
@echo off
echo enumerating fragment shaders...
for %%I in (*_frag.glsl) do (
    echo %%I
    @echo on
    glslc -fshader-stage=fragment %%I -o %%~nI.spv
)
@echo off
echo enumerating vertex shaders...
for %%I in (*_vert.glsl) do (
    echo %%I
    @echo on
    glslc -fshader-stage=vertex %%I -o %%~nI.spv
)

@echo off
echo shader enumeration and conversion done