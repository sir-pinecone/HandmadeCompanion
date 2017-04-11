---
title: "Translation Units, Function Pointers, Compilation, Linking, and Execution"
videoId: "n4fI4eUTTKM"
markers:
    "0:08": "Set the stage for the chat"
    "1:45": "Read @Nacre314's email regarding typedef'ing of functions"
    "6:59": "A few words on C never having reached maturity on the relationship between code and data"
    "9:29": "On departing from batch files"
    "12:50": "Create nacre314 directory and main.cpp"
    "15:39": "Compilation vs Linking"
    "17:52": "The compiler as an auto-detecting stream processor"
    "21:14": "Create main2.c, and #include it in main.cpp as a way to make the compiler aware of this file"
    "26:16": "Translation units, as evinced by passing individual files to the compiler, versus using the #include approach"
    "32:42": "C, as opposed to C++, can assume which arguments a function takes without having seen its declaration"
    "38:58": "Linking"
    "45:33": "Looking at our linker output and our compiled .obj files"
    "48:44": "\"I don't know how to use cars\" (!quote 566)"
    "48:55": "Use dumpbin to inspect main.obj, and note that Bar() is without a known address"
    "56:09": "Inspect main3.obj, with its corresponding indicator for Bar()"
    "58:59": "The linker prepares an executable that conforms to the OS standard"
    "1:00:47": "Call main() in main.cpp and successfully link"
    "1:03:12": "C runtime standard library, as implicitly included by Windows"
    "1:08:42": "Set LIB to nothing and note that linking now fails"
    "1:12:04": "Create build.bat to control each step in the compilation / linking process"
    "1:17:16": "Entry point, mainCRTStartup()"
    "1:23:49": "C++ name mangling"
    "1:26:44": "Use extern \"C\" to make the compiler comply with the C rules"
    "1:29:33": "Specifying the SUBSYSTEM to enable the linker to recognise this as the entry point on Windows"
    "1:35:01": "Inspect the .exe output of the linker"
    "1:39:15": "Function pointers"
    "1:42:48": "main.cpp: Introduce void *FunctionPointer = Foo() and inspect the .obj file"
    "1:47:30": "Dereferencing a function pointer"
    "1:49:32": "Relative addressing and Address Space Layout Randomization (ASLR)"
    "1:53:48": "Manually deference the function pointer pointing to Foo()"
    "1:57:34": "main.cpp: Introduce Five() and watch it operate in the debugger"
    "2:03:18": "How C calls a function [see Resources, Wikipedia]"
    "2:07:47": "Using typedef on the function signature in order to enable the compiler to generate the preamble to a call instruction"
    "2:14:11": "main.cpp: Introduce CodeForFive[] to contain the code for Five() as an array of characters"
    "2:16:58": "Inspect the .data section in our COFF .obj file"
    "2:24:08": "Step through our call to CodeForFive[] and hit the access violation for the attempt to execute code not marked as executable"
    "2:26:43": "VirtualAlloc() and Memory Protection Constants [see Resources, MSDN]"
    "2:35:13": "Step through our call to VirtualAlloc() and on to CodeForFive[]"
    "2:38:58": "main.cpp: Enable CodeForFive[] to return any 4-byte value"
    "2:42:49": "Relative calls, using address offsets and function sizes"
    "2:55:23": "main.cpp: Rename CodeForFive[] to Code[] and enable it to perform a relative call to Five()"
    "3:01:39": "Step through our generated relative call to Five()"
    "3:04:16": "The code that the compiler generates is just bytes"
    "3:05:48": "Multiple separate executable pieces"
    "3:07:57": "How programs were executed historically, before multitasking operating systems"
    "3:10:53": "Secondary link phase that happens upon loading an executable"
    "3:12:48": "Import libraries, kernel32.lib as a dynamically linked library (dll), and what Windows' dynamic linker does"
    "3:22:38": "Compare the hex dump of main.exe with the running code after being dynamically linked"
    "3:35:46": "The linker links with an expectation of what the base address probably would be"
    "3:37:05": "Continue to compare the raw data before and after dynamic linking"
    "3:41:38": "How Handmade Hero uses Windows' mechanism for dynamic linking in order to perform hot reloading"
    "3:44:31": "Inspect the dumpbin imports and exports for win32_handmade.exe and handmade.dll"
    "3:48:17": "Portable Executable [see Resources, Wikipedia]"
    "3:50:01": "The CPU is executing code somewhere, indicated by the contents of the RIP register"
    "3:54:13": "The stack location is referenced off the RSP register"
    "3:55:37": "Q&A"
    "3:56:24": "pseudonym73 Q: Worth a plug [see Resources, The Invincible Electric Calculator Company]"
    "3:57:20": "butwhynot1 Q: Technically, you need to call FlushInstructionCache when you generate code"
    "3:59:26": "FlushInstructionCache [see Resources, MSDN]"
    "4:01:36": "main.cpp: Demo usage of FlushInstructionCache()"
    "4:08:35": "insofaras Q: I think something like /SECTION:.data,RWE and /NXCOMPAT:NO to link.exe might work for the non-VirtualProtect code in array thing"
    "4:09:30": "nacre314 Q: Maybe this episode belongs in the Intro to C week as episode 6?"
    "4:09:39": "user549 Q: Maybe the smallest crt ever [see Resources, suckless] "
    "4:10:02": "filiadelski Q: Is there a low-level reason you don't like virtual functions? My understanding of them is that they're basically just function pointers in a table that is dereferenced"
    "4:11:45": "ingenero Q: First of all, thank you so much for this stream! At the beginning you mentioned you hadn't done some of these things for a while. My question is, in what context did you originally learn and apply all this information? It seems like spending time at work exploring this may be considered time taken away from actually being \"productive\". Is it just something you have to learn on your own time?"
    "4:15:07": "Wrap it up there"
---

## Quotes

* 566\. I don't know how to use cars (48:44)

## Resources

* The Invincible Electric Calculator Company, [Linkers and Loaders](https://www.iecc.com/linker/) 
* MSDN, [FlushInstructionCache](https://msdn.microsoft.com/en-us/library/windows/desktop/ms679350)
* MSDN, [Memory Protection Constants](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366786)
* MSDN, [VirtualAlloc](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366887)
* suckless, [suckless C compiler/crt.s](http://git.suckless.org/scc/tree/crt/amd64-sysv-linux/crt.s)
* Wikipedia, [Application binary interface](https://en.wikipedia.org/wiki/Application_binary_interface)
* Wikipedia, [Portable Executable](https://en.wikipedia.org/wiki/Portable_Executable)
