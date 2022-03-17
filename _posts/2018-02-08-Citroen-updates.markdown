---
layout: post
date:   2018-02-08 13:22:16
title: "Citroen C4 Picasso software and map updates"
categories: personal
---

I shouldn't have to write this but I thought it may be handy to someone. I had a terrible time trying to figure out how to update the software on our 2016 Citroen C4 Picasso so I thought I'd write a guide how to get them. Hoping somebody will find this through google and save themselves some time.

You get a few years of free map updates with this car and also software updates are available.

To get the updates do the following

1. Go to [My Citroen UK](https://mycitroen-uk.citroen.com/MyCitroen/?page=).
2. Create a new account. 
3. Click on [My Car](https://mycitroen-uk.citroen.com/MyCitroen/?page=gerer_mes_vehicules/vehicle).
4. Add a vehicle using the VIN (vehicle identifier number).
5. Once the vehicle is added hover over "Citroen Shop" at the top, then click on "Citroen Connect Nav".
6. You end up on [this page](https://mycitroen-uk.citroen.com/MyCitroen/?page=[ovip_without_bta]_page_telechargement_applicatif).
7. Download the instructions, map updates and software updates.
8. Follow the instructions (these are complex as well, but the instructions are clear unlike finding the files themselves - see below).

Edit: if the instructions above don't give you the files, try this link: https://github.com/ludwig-v/psa-nac-firmware-reverse-engineering

[This page](http://citroen.cricou.net/NAC/Tutorial_NAC_Mise_à_jour_firmware_ou_cartographie.pdf) in french is handy. [This page in English](http://c4owners.org/plugins/forum/forum_viewtopic.php?226475) and [this page](https://en.discussions.tomtom.com/built-in-car-navigation-25/citroen-c4-picasso-connect-nav-usability-1010966/index5.html) also look handy!

The instructions are [here](https://mycitroen-uk.citroen.com/Resources/Content/GB/ESP/04_Services/ConnectNAV/Citroën_Navigation_Map_Download_Instruction_Manual.pdf).

The map file is called PSA_map-eur_7.0.0-NAC_EUR_WAVE2.tar

The software file is called PSA_ovip-int-firmware-version_21-06-47-34_NAC-r0_NAC_EUR_WAVE2.tar. It is version Version V.21.06.47.34_NAC-r0.

For context, the reason I decided to make this page was the very confusing instructions on Citroen's website. Hopefully somebody from Citroen will solve this usability problem. The steps I went through to try to find the maps was roughly:

1. Go to [Citroen's UK website](http://www.citroen.co.uk/home)
2. Find the [FAQ about map updates](https://citroen-uk-en.custhelp.com/app/answers/detail/a_id/9222)
3. It sends me to [Citroen navigation](http://citroen.navigation.com/).
4. I make an account there.
5. I log into my account there.
6. I register my car there. 
7. I go to the "Find Product" page but it tries to charge me for the maps.
8. I go back to the homepage and click on my car "Citroen C4 Picasso".
9. I click on connect nav which is what my car uses.
10. It says "Please connect to your Citroen.com Personal Account or visit your distributor to get your navigation system map updates."
11. I have already been at Citroen.com and it sent me here, so I call my dealer.
12. They cannot install map updates for me.
13. I have to call customer support to try to figure it out but they say "just follow the instructions".
14. Eventually I use google to find "My Citroen". Cannot remember how I found it.
15. Eventually after much hunting I find the route to the files given at the top of this page.

Please Citroen, make this easier!
