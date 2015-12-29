---
layout: post
date:   2015-12-29 15:22:16
title: "Tool for making a supply of strangely named files"
categories: go programming
---

I made a tool a couple of months ago that helps to create a supply of really strangely named files and folders.

A video is better than a thousands words, so here goes:

<a class="embedly-card" href="http://www.youtube.com/watch?v=6befxr1jYL0">replacer</a>
<script async src="//cdn.embedly.com/widgets/platform.js" charset="UTF-8"></script>

This has been useful for my work for Adobe on a file sync program. It has to be able to sync arbitrary file names on both Mac and Win. We need to make sure it works with all these characters, as in the past we've had problems occur during development for files containing characters that

* are non-ASCII
* are non-European
* contain symbols
* have long Unicode representations such as [supplementary plane characters]
(https://en.wikipedia.org/wiki/Plane_(Unicode)#Supplementary_Multilingual_Plane)
* have different normalisation forms in NFD, NFC, etc. such as characters with accents
* and so on.

This tool currently creates folders and files with names containing all these types.

You can download it for Windows or Mac [here](https://github.com/neil-ca-moore/language-tool/tree/master/bin).

The source is [here](https://github.com/neil-ca-moore/language-tool).

You use it by downloading it (or building it by following the instructions at github) and then doing something like

{% highlight bash linenos %}
./language-tool --path=~/tmp/weirdfiles 
{% endhighlight %}

Then it creates an infinite supply of really strange names. Every time you move or delete a file from ~/tmp/weirdfiles, it is replaced by another one from the same category. The start of the name tells you the category of weird characters it uses.

E.g. the file Arabic_ۺﲳﵫ𞸊ﶎ𐹲ݔﮏٝ𞹰.txt is a text file whose name contains random Arabic characters.

I envisage that some people will want to have it running all the time, and use it whenever they need a test file/folder. Just cut and paste, or drag it somewhere.

It is likely to be useful for testing sync programs, virtual file systems, file browsers and any program with a file picker in it!

Hope you like it.

Pull requests for changes are welcome.
