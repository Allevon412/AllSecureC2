# AllSecure


## Introduction
This repository is my way of increasing my skills as a developer while at the same time increasing my skills as a red teamer and penetration tester. I have always had a passion for red teaming, and I wanted to really dive deep into the weeds of learning how things work on the backend when you're performing an assessment. Therefore, I have created a C2 server named AllSecure.

## What is AllSecure?
This server is pretty much a "Havoc Framework" at home type of situation. Because I did not come from a development background so many of the topics, I've learned over the course of this project's development I've learned from reverse engineering the havoc framework and reading maldevacademy's guides. I cannot recommend enough Spider’s and Maldevacademy’s work. Spider is a freakin genius and was the inspiration for this project. Some of my code is almost a 1 for 1 replica of the code in Havoc because he's just so much better of a developer that I couldn't even think of a way to do it better. An example of this is the parser feature for the configuration data in the implant and the implant builder in the C2 server. 

While I made modifications where I could or fit it to my own needs, the content of Havoc is just extremely well written and therefore I considered using the techniques as learning those development practices. I wouldn’t have been able to come up with the parser feature without seeing it in Havoc first. That being said I hope explaining my decisions in this matter is enough credit for you to check out Spider's work as it was such a huge inspiration for my own. This project is also something I can only do in my off time as a I work / have a pretty busy day to day routine with pets, exercise, etc. This project has taken me months of dedication, of trying to just do a little bit everyday to get it to a point where I felt comfortable enough to publish it. This is also the biggest project I've ever taken on which was one of my main motivating factors as I really wanted to work on my development skills. So, please be don't be too quick to judge harshly. 

## Differences
Where does AllSecure Differ from Havoc or other C2 frameworks? Well, there's a few features that are different and that I am proud of. Not that they're new techniques to the industry, but many non-commercial C2 frameworks don't seem to have these features from what I’ve seen.

1. The AllSecure communications are sent over HTTP or HTTPS. However, in addition to using this communication protocol the data is encrypted via AES-256, which is standard practice. My implementation differs by using RSA to encrypt the AES key that is sent in the original meta-data registration package. When the team server generates the implant, part of the configuration bytes is a public key that is used to encrypt the AES key when the data is transmitted across the network. That way even in environments where web proxies are used that have HTTPS decryption / packet inspect capability the AES key cannot be extracted via reviewing the traffic. This is the same methodology used in Cobalt Strike. When the package is received by the listening server, the listener first decrypts the AES key using a private key that is generated when the implant is built. Then if that is successful the package is parsed for the AES key and the AES key / IV is used to decrypt the rest of the data. The only way your traffic can ever be decrypted is if access to the private key is obtained. However, this should not be possible. Only the registration package will ever send the encrypted AES key / IV.
   - The library's I am using to perform this encryption / decryption process is the WOLFSSL library. However, I've ripped out the code necessary to perform the RSA encryption and the AESCTR encryption / decryption (which is the same function). This has significantly increased the implants size unfortunately. My reasoning for this is because I didn't want to have statically compiled libraries for the need of PIC. Additionally, I didn't want to have more callbacks than necessary and download / load libraries which may cause extra detections. So, I opted for this tradeoff of having a larger binary. Why is it so much larger? That is because in order to perform PKI operations most crypto libraries such as TomCrypt, EmbedTLS, etc. generally have a separate numbers / math library that is necessary for working with such larger integers required for PKI. This means you'd not only have to download and load the crypto library if you dynamically linked, but also the numbers/math library. While this is a viable option, I’ve found that WOLFSSL library has it packaged within one library and therefore I ripped out whatever code I needed and called it a day. Now, I could also probably go line by line and cut out whatever content I do not need, but that is very monotonous work for little reward I think so I opted not to do it. If you're reading this and you want to perform that task and create a fork, please let me know and I'll add it to the original project.
2.	The configuration bytes that are added to the implant are encrypted using AES. The AES key is also added along with the configuration bytes during build time which means that a reverse engineer could easily find the key / iv and the configuration. However, it's one more layered step that is required to find the data.
3. The implant uses the silentmoonwalk technqiue for stack spoofing. This is implemented while the project sleeps and calling certain key APIs.
4. The implant uses my method of SyscallTempering (a small PoC can be found in my GitHub Repo's). Which utilizes hardware breakpoints to call system calls in a stealthy manner. ( my repo has a much more detailed explanation).

 ## RoadMap
- I want to implement my own authentication scheme when the implant connects to the listening server. I.e. uses specific User-Agent, Specific IP address expected, specific authentication code or something similar.
- The code only works as a 64bit executable at the moment, there will be support in the future for different versions i.e. shellcode, dll, 32bit of each format, loaders.
- I would like to implement a custom packer as well.
- reflective loader capability
- evasive loader grab main functionality of agent
- indirect syscalls - we use hardware breakpoints instead of indirect syscalls.
- unhooking
- amsi & etw patching
- persistence techniques
- injecting into 32bit & 64bit processes
- tunneling / socks proxying
- redirectors?
- stomp peheader and create custom loader to handle it.


This project will be updated only as a way for me to learn new techniques and see how they incorporate into the bigger picture of an entire C2 framework. For example, when a C2 server tells the implant to perform reflective loading of a DLL or PE file, what does that look like? When an implant is set up to use a secondary heap and the heapallocate function calls are hooked to point to the secondary heap so that secondary heap can be encrypted during implant sleep cycle, what does that look like? These are examples of techniques I am using for the project as an excuse to learn. Therefore, I will incorporate whatever techniques I desire to learn into the framework and will keep the project going that way. In this manner I will no longer have to create individual tools and repos for each new technique but instead incorporate it into a system where they all work together.

## What this project is not:

This project is not supposed to be a replacement for commercial C2 frameworks or a replacement for something like Havoc. Maybe one day when I become a better developer, I will have the confidence to say that my project should be used in an assessment, however, I believe you might be better off using those projects instead. But I do believe this is a great resource for any of you who like me just had a curiosity for learning and just wanted to do something. Unfortunately, to get a lot of the skillsets and knowledge required I had to reverse engineer someone else's work, but I also do not have the opportunity to learn these things from people I personally know or at work.

Welcome to AllSecure, where suggestions are always welcome, and I hope you enjoy my work.

