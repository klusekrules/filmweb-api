COPY /B /Y ..\Laucher\config.txt ..\out\bin\config.txt
COPY /B /Y ..\dll\%1\libcurl.dll ..\out\bin\libcurl.dll
COPY /B /Y ..\dll\%1\libcurl_debug.dll ..\out\bin\libcurl_debug.dll
COPY /B /Y ..\dll\%1\libeay32.dll ..\out\bin\libeay32.dll
COPY /B /Y ..\dll\%1\ssleay32.dll ..\out\bin\ssleay32.dll