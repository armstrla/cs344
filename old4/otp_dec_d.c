/****************************************************************************

otp_dec_d:

This program performs exactly like otp_enc_d, in syntax and usage. In this 
case, however, otp_dec_d will decrypt ciphertext it is given, using the 
passed-in ciphertext and key. Thus, it returns plaintext again to otp_dec.

****************************************************************************/

/****************************************/
/*	otp_dec_d: ("D" STANDS FOR DAEMON)	*/
/*										*/
/* 	Example usage: 						*/
/*										*/
/*		otp_enc_d 57172 &				*/
/****************************************/

int main(int argc, char *argv[]){
	// run in the background as a daemon

	// listen on assigned port

	// Once connected to otp_dec, forks off a separate process and is available to receive more connections
	// (must support 5 concurrent socket connections)

	// receive ciphertext and key via port (from otp_enc)

	// perform the actual decoding inside forked off process

	// Check that key is at least as long as ciphertext

	// write back plaintext to connected process via same port



	return 0;
}