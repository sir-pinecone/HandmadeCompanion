---
title: "Translation Units, Function Pointers, Compilation, Linking, and Execution"
videoId: "n4fI4eUTTKM"
markers:
    "48:44": "\"I don't know how to use cars\" (!quote 566)"
    "3:55:57": "Q&A"
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
* MSDN: [FlushInstructionCache](https://msdn.microsoft.com/en-us/library/windows/desktop/ms679350)
* suckless, [suckless C compiler/crt.s](http://git.suckless.org/scc/tree/crt/amd64-sysv-linux/crt.s)
