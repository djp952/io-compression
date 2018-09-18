# __io-compression__

Managed C++/CLI library for compression and decompression of BZIP2, GZIP, LZ4, LZMA and XZ formatted streams.
  
Copyright (C)2016-2018 Michael G. Brehm    
[MIT LICENSE](https://opensource.org/licenses/MIT) 
  
[__BZIP2__](http://www.bzip.org/) - Copyright (C)1996-2010 Julian R Seward  
[__LZ4__](http://lz4.github.io/lz4/) - Copyright (C) 2011-2016, Yann Collet  
[__LZMA SDK__](http://www.7-zip.org/sdk.html) - LZMA SDK is written and placed in the public domain by Igor Pavlov  
[__ZLIB__](http://www.zlib.net) - Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler  
  
LZO compression [__http://www.oberhumer.com/opensource/lzo/__](http://www.oberhumer.com/opensource/lzo/) is not included in this library as its source code is distributed under the terms of the [__GNU General Public License__](http://www.oberhumer.com/opensource/gpl.html).  
  
## __NUGET PACKAGES__  
zuki.io.compression is available as a public nuget package:  
   
[__zuki.io.compression__](https://www.nuget.org/packages/zuki.io.compression/)  
```
PM> Install-Package zuki.io.compression
```
   
## __BUILD__  
### __System Requirements__  
* Microsoft Visual Studio 2017 Community Edition (or higher)   

### __Source Code Repositories__  
[__djp952/io-compression__](https://github.com/djp952/io-compression)    
```
git clone https://github.com/djp952/io-compression
git submodule update --init
```
### __Build Win32 release binaries__  
Open Developer Command Prompt for VS2017  
```
devenv io-compression.sln /Build "Release|Win32" /project compression
  
> out\Win32\Release\zuki.io.compression.dll
```
### __Build x64 release binaries__  
Open Developer Command Prompt for VS2017  
```
devenv io-compression.sln /Build "Release|x64" /project compression

> out\x64\Release\zuki.io.compression.dll
```
