# recycle-bin

> Move files and folders to the Windows recycle bin

## Install

[Download](https://github.com/GeorgeTR1/recycle-bin/releases/latest) the binary and put it somewhere in your `%path%`

## Usage

```
$recycle --help

  Move files and folders to the recycle bin

  Usage: recycle (--version | --help | <path>...)
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

Then follow the instructions [here](https://superuser.com/a/1134468) to apply these macros every time you start a command prompt. If you use PowerShell or some other terminal instead there may be other ways of achieving this; you would have to figure that out yourself.

## Reasons for this fork

This fork is based on version 1.0.1 of the upstream repo, the source that I used is available [here](https://github.com/sindresorhus/recycle-bin/blob/b34fedfc16a7fe0f8a28c419ac722706f882267b/recycle-bin.c). After that they switched to using the `IFileOperation` interface instead of the `SHFileOperationW` command. In my opinion there's not much reason to do that for this use case, regardless of what Microsoft might recommend. One downside I can see is that it doesn't support long path names. If that's a significant issue for you, you can use the upstream latest version, but neither cmd.exe nor PowerShell 5.1 work for long file paths anyway (maybe newer PowerShell does? Or the new Windows Terminal?). But at least on my machine, the downside was it was a lot slower, or at least, it had a longer startup time. I found it annoying that just recycling a single empty file meant a significant delay. Plus, it just makes the implementation way more complicated, and there's the added benefit that this can work for versions of Windows prior to Vista.

There are a couple of additional improvements to that version of the upstream repo, though. I added error reporting—previously if any of the files you wanted to delete weren't found or couldn't be deleted because you didn't have access, the command just returned silently. Only if you inspected the return value (like running `echo %errorlevel%`) would you know something went wrong. Now the command should report errors properly.

Also, I removed all references to the C standard library and used pure WinAPI calls instead. This wasn't particularly difficult, and means that the program can be compiled into a tiny executable (only 4kB!) while only depending on sytem dlls.

### Running on Old Systems

Another reason for this fork is that this code should be able to run on even the oldest 32-bit Windows systems, and I've made several changes to help make that possible. All of the functions used here have been around since Windows 95, but after doing testing on my Windows XP machine, I found it wasn't working properly, specifically it was permanently deleting files rather than moving them to the recycle bin. I was able to find a very [similar utility](https://web.archive.org/web/20231128043232/http://www.maddogsw.com/cmdutils/) made by Matt Ginzton in 1998, and in the source code he mentions that there is a quirk with `SHFileOperation` where you must pass the full pathnames of each file to `SHFileOperation` for recycling to work. This quirk no longer exists in modern Windows, but it did as recently as XP, maybe even more recently than that. 

After changing the code to pass full pathnames, it now works on my Windows XP machine. To clarify, the binary in the releases won't work, it's not even 32 bit, but I compiled the code on my XP machine using Borland C++ 5.5, and with very minimal changes (just renaming `mainNoCRT` to `main` and removing the `#pragma` directives) it compiles and runs just fine. Unfortunately there's no way I know of to avoid statically linking with the C runtime with that toolchain, so the file ends up being a bit bigger, just over 50 kB. If anyone has an interest in that build, let me know and I can add it to the releases.

After making those changes, I believe that the code should work as far back as NT 4.0. Support for 9x would require a bit of work removing references to Unicode, but nothing too crazy.

## Build

The source code is just a single C file. Other than the three `#pragma` directives that specify the libraries that the program needs to link with, nothing is compiler specific, so it should compile in any C compiler that targets Windows if you remove those and link with those libraries in a different way. But, to build just how I did and get a tiny executable, do the following:
1. Download `cc.bat` and `recycle.c` into the same directory (cloning the repository will achieve this, of course)
2. Open a command prompt where you can use MSVC. This can be with Visual Studio, Visual Studio build tools, or using the instructions [here](https://gist.github.com/mmozeiko/7f3162ec2988e81e56d5c4e22cde9977).
3. Navigate to the directory where you downloaded the files
4. Run `cc recycle.c`

## Related

- [Trash Management on ArchWiki](https://wiki.archlinux.org/title/Trash_management#Command_line) - Information about various trash cli tools, mainly for linux, but several are cross-platform

## Credits

- This [gist](https://gist.github.com/mmozeiko/81e9c0253cc724638947a53b826888e9) by [@mmozeiko](https://github.com/mmozeiko) for information on how to make compile tiny Windows binaries using MSVC
- *Programming Windows, 5th Ed.* by Charles Petzold for general information on how to program with the Win32 API
- [Windows 95 API reference](https://archive.org/download/nt40_win95_sdk1996/nt40_win95_sdk1996.iso/DOC%2FHLP%2FWIN32SDK.HLP), available on the Internet Archive as a `.HLP` file (see [listing](https://archive.org/details/nt40_win95_sdk1996)). For help opening `.HLP` files on modern systems, see [here](https://eileenslounge.com/viewtopic.php?p=281561#p281561)
- Members of the [r/C_Programming](https://www.reddit.com/r/C_Programming/) subreddit, specifically [u/skeeto](https://www.reddit.com/user/skeeto/) and [u/mikeblas](https://www.reddit.com/user/mikeblas/), who gave gave me advice and suggestions about my build setup in a [post](https://www.reddit.com/r/C_Programming/comments/1vdju83/helpful_windows_gui_program_in_60_lines_of_c/) I made about a similar project
- [@sindresorhus](https://github.com/sindresorhus) and [@Qix-](https://github.com/Qix-) for their contributions to the original [recycle-bin](https://github.com/sindresorhus/recycle-bin) project this is fork of
- Matt Ginzton for his [utility](https://web.archive.org/web/20231128043232/http://www.maddogsw.com/cmdutils/) made in 1998 that does much the same thing this does. In the [source code](https://web.archive.org/web/20231128043232/http://www.maddogsw.com/cmdutils/cmdutils.zip) he provides, he leaves a comment saying,

  > ...the one quirk [in `SHFileOperation`] is undo information can be preserved (meaning we delete to the recycle bin instead of a normal delete) only if we pass the full pathname of each file.

  That was the key I needed to get this working on old systems

----
*No generative AI was used in any part of the creation of this software.*
