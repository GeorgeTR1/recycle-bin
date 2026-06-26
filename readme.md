# recycle-bin

> Move files and folders to the Windows recycle bin

## Install

[Download](https://github.com/GeorgeTR1/recycle-bin/releases/latest) the binary and put it somewhere in your `%path%`

## Usage

```
$ recycle --help

Usage: recycle <path> [...]
```

## Encouraging Use

You may be very used to using the `del` and `rd` commands, etc., and habits are hard to break. To encourage the use of this instead, you can copy the following into a file, save it anywhere with a name like `macros.doskey`. Note that I decided to rename the executable `rt` for brevity (for "remove trash", meant to be an alternate form of `rm`, see [here](https://github.com/oberblastmeister/trashy#should-i-alias-rmtrashy-put)). The "f" in `delf` means force. You can change instances of "rt" to "recycle" or something else if you prefer.

```
delf=del $*
  del=echo off $t echo Permanent deletion not recommended. Use "rt" instead. $t echo If this is what you meant, use "delf". $t echo on
erase=echo off $t echo Permanent deletion not recommended. Use "rt" instead. $t echo If this is what you meant, use "delf". $t echo on
rdf=rd $*
   rd=echo off $t echo Permanent deletion not recommended. Use "rt" instead. $t echo If this is what you meant, use "rdf". $t echo on
rmdir=echo off $t echo Permanent deletion not recommended. Use "rt" instead. $t echo If this is what you meant, use "rdf". $t echo on
```

Then follow the instructions [here](https://superuser.com/a/1134468) to apply these macros every time you start a command prompt. If you use PowerShell or some other terminal instead there may be other ways of achieving this, you would have to figure that out yourself.

## Reasons for this fork

This fork is based on version 1.0.1 of the upstream repo, the source that I used is available [here](https://github.com/sindresorhus/recycle-bin/blob/b34fedfc16a7fe0f8a28c419ac722706f882267b/recycle-bin.c). After that they switched to using the `IFileOperation` interface instead of the `SHFileOperationW` command. In my opinion there's not much reason to do that for this use case, regardless of what Microsoft might recommend. One downside I can see is that it doesn't support long path names. If that's a significant issue for you, you can use the upstream latest version, but neither cmd.exe nor PowerShell 5.1 work for long file paths anyway (maybe newer PowerShell does? Or the new Windows Terminal?). But at least on my machine, the downside was it was a lot slower, or at least, it had a longer startup time. I found it annoying that just recycling a single empty file meant a significant delay. Plus, it just makes the implementation way more complicated.

There are a couple of additional improvements to that version of the upstream repo, though. I added error reporting—previously if any of the files you wanted to delete weren't found or couldn't be deleted because you didn't have access, the command just returned silently. Only if you inspected the return value (like running `echo %errorlevel%`) would you know something went wrong. Now the command should report errors properly.

Also, I removed all references to the C standard library and used pure WinAPI calls instead. This wasn't particularly difficult, and means that the program can be compiled into a tiny executable (only 4kB!) while only depending on sytem dlls. It should also compile for ancient versions of Windows, if you use the proper compiler, if that's something you'd be interested in.

## Build

The source code is just a single C file. Other than the three `#pragma` directives that specify the libraries that the program needs to link with, nothing is compiler specific, so it should compile in any C compiler that targets Windows if you remove those and link with those libraries in a different way. But, to build just how I did and get a tiny executable, do the following:
1. Download `cc.bat` and `recycle-bin.c` into the same directory (cloning the repository will achieve this, of course)
2. Open a command prompt where you can use MSVC. This can be with Visual Studio, Visual Studio build tools, or using the instructions [here](https://gist.github.com/mmozeiko/7f3162ec2988e81e56d5c4e22cde9977).
3. Navigate to the directory where you downloaded the files
4. Run `cc recycle-bin.c`

## Related

- [Trash Management on ArchWiki](https://wiki.archlinux.org/title/Trash_management#Command_line) - Information about various trash cli tools, mainly for linux, but several are cross-platform
