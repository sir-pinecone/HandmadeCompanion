@echo off
cl -nologo -DPLATFORM_WINDOWS=1 -FC handmade_srt_gen.cpp kernel32.lib
