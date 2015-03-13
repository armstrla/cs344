/****************************************************************************

otp_enc_d: 

This program will run in the background as a daemon. Its function is to perform 
the actual encoding, as descripted above in the Wikipedia quote. This program 
will listen on a particular port, assigned when it is first ran, and receives 
plaintext and a key via that port when a connection to it is made. It will then 
write back the ciphertext to the process that it is connected to via the same 
port. Note that the key passed in must be at least as big as the plaintext.

When otp_enc_d makes a connection with otp_enc, it must fork off a separate 
process immediately, and be available to receive more connections. Your version 
must support up to five concurrent socket connections. In the forked off process, 
the actual encryption will take place, and the ciphertext will be written back.

You may either create a new process every time a connection is made, or set up a 
pool of five process to handle your encryption tasks. Your system must be able to 
do five separate encryptions at once, using either method you choose.

Use this syntax for otp_enc_d:

otp_enc_d listening_port


****************************************************************************/

int main(){

	return 0;
}