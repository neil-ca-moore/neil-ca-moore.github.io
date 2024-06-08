---
layout: post
date:   2024-06-07 13:22:16
title: "Delayed mail in Gmail"
categories: personal
---

I have implemented a productivity hack so that my email is only delivered twice a day, 
once between 8 and 9am, and later between 5 and 6. That way I don't check it when I wake up
and I don't check it during the day. It's more like the experience when you get home and
the postman has been and you can look at all your mail together.

I have managed to completely eliminate compulsive checking of emails. I know there will be
nothing new there most of the time.

I use Gmail as a mail client. The way that I have achieved this is:

1. To archive all incoming email, but apply a tag calling "snoozing". That means it doesn't show in my
Inbox.

2. To apply label "snoozing" to all sent email. That means it appears in my inbox later on, and I can
decide if it needs a follow up.

3. To write a "Google Apps Script" program that moves all "snoozing" tagged emails to the Inbox and removes
the "snoozing" tag. It runs at 8-9am and 5-6pm.

## Implementation

The incoming filter is:

```
Matches: larger:1
Do this: Skip Inbox, Apply label "snoozing"
```

i.e. it matches all emails whose size is 1 byte or more, and it does "Skip the inbox" and "Apply label snoozing".

The outgoing filter is:

```
Matches: from:me
Do this: Apply label "snoozing"
```

i.e. it just applies the label to emails from me.

The Google Apps Script is created at https://script.google.com/home/.

Create a new project and enter the following code:

```
function myFunction() {
  GmailApp.moveThreadsToInbox(GmailApp.search('label:snoozing'));
  var label = GmailApp.getUserLabelByName("snoozing");
  var threadArray = GmailApp.getInboxThreads();
  for (var i=0; i< threadArray.length; i++) {
    threadArray[i].removeLabel(label);
  }
}
```

Add a "Service" for Gmail API.

Then go to "Triggers" and choose time based triggers for times that will suit you.

Save it.

Do some test runs.
