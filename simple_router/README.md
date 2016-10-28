Note: Code written in 2011!

**About**  
Based on the [Stanford VNS simple router](http://yuba.stanford.edu/vns/assignments/simple-router/) project; all [requirements](http://yuba.stanford.edu/vns/assignments/simple-router/#Required_Functionality) used/met except for enforcement of guarantees on timeouts.  
Stub code: All source files except sr_router.c.  
Code I added: Everything in sr_router.c, except the signature of sr_ForwardPacket.  
This code is intended as a sample and cannot, unfortunately, be run without a user account on the VNS. However, basic information on compiling and running is provided for context.  

**Compiling**  
$ make  

**Running**  
$ ./sr -s vns-2.stanford.edu -u <userid> -t <topoid>  
For help:  
$ ./sr --help  

**Cleanup**  
$ make clean  
