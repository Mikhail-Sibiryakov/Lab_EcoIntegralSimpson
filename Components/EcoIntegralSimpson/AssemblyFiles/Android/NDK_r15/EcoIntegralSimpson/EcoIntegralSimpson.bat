call ndk-build clean
call ndk-build TARGET_LINK=0 NDK_DEBUG=0 CONFIGURATION=StaticRelease
call ndk-build TARGET_LINK=0 NDK_DEBUG=1 CONFIGURATION=StaticDebug
rmdir /s /q libs
rmdir /s /q obj
call ndk-build clean
call ndk-build TARGET_LINK=1 NDK_DEBUG=0 CONFIGURATION=DynamicRelease
call ndk-build TARGET_LINK=1 NDK_DEBUG=1 CONFIGURATION=DynamicDebug
rmdir /s /q libs
rmdir /s /q obj
rem xcopy /y ..\..\..\..\BuildFiles\Android\arm64-v8a\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\arm64-v8a\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\arm64-v8a\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\arm64-v8a\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\armeabi\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\armeabi\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi-v7a\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\armeabi-v7a\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\armeabi-v7a\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\armeabi-v7a\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\mips\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\mips\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips64\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\mips64\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\mips64\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\mips64\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\x86\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\x86\DynamicRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86_64\StaticRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.a %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\x86_64\StaticRelease\
rem xcopy /y ..\..\..\..\BuildFiles\Android\x86_64\DynamicRelease\libBA973067BA954E8FA50BDDE55BC4E3DF.so %ECO_FRAMEWORK%\EcoIntegralSimpson\BuildFiles\Android\x86_64\DynamicRelease\
pause
