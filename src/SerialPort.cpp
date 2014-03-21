/*
 * SerialPort.cpp
 *
 *  Created on: Jun 20, 2011
 *      Author: jose
 */

#include <string.h>
#include <sys/ioctl.h>

#include "SerialPort.h"

int SerialPort::connect(const char device[]) {
	struct termios terminalAttributes;

	/*
	 * http://linux.die.net/man/2/open
	 *
	 * Open the serial port
	 * read/write
	 * not become the process's controlling terminal
	 * When possible, the file is opened in nonblocking mode
	 *
	 */
	fileDescriptor = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_FSYNC );

	// clear terminalAttributes data
	memset(&terminalAttributes, 0, sizeof(struct termios));

	/*o resto do codigo pode ser trocado por:
	 *
	 *char command[]=	"stty -F /dev/ttyACM0 cs8 115200 ignbrk -brkint -icrnl -imaxbel"
	 *	 			" -opost -onlcr -isig -icanon -iexten -echo -echoe -echok "
	 *				"-echoctl -echoke noflsh -ixon -crtscts";
	 *
	 *system(command);
	 */

	/*	http://linux.die.net/man/3/termios
	 *
	 *  control modes: c_cflag flag constants:
	 *
	 * 115200 bauds
	 * 8 bits per word
	 * Ignore modem control lines.
	 * Enable receiver.
	 */

	terminalAttributes.c_cflag = B115200 | CS8 | CLOCAL | CREAD;

	/*
	 * input modes: c_iflag flag constants:
	 *
	 * Ignore framing errors and parity errors.
	 * (XSI) Map NL to CR-NL on output.
	 */
	terminalAttributes.c_iflag = IGNPAR |  ONLCR;

	/*
	 * output modes: flag constants defined in POSIX.1
	 *
	 * Enable implementation-defined output processing.
	 */

	terminalAttributes.c_oflag = OPOST;

	/*
	 * Canonical and noncanonical mode
	 *
	 * min time
	 * min bytes to read
	 */

	//terminalAttributes.c_lflag = ICANON;
	terminalAttributes.c_cc[VTIME] = 0;
	terminalAttributes.c_cc[VMIN] = 1;

	/*
	 * http://linux.die.net/man/3/tcsetattr
	 * Set the port to our state
	 *
	 * the change occurs immediately
	 */

	tcsetattr(fileDescriptor, TCSANOW, &terminalAttributes);

	/*
	 * http://linux.die.net/man/3/tcflush
	 *
	 * flushes data written but not transmitted.
	 * flushes data received but not read.
	 */

	tcflush(fileDescriptor, TCOFLUSH);
	tcflush(fileDescriptor, TCIFLUSH);

	return fileDescriptor;
}

void SerialPort::disconnect(void)
{
    close(fileDescriptor);
    printf("\nPort 1 has been CLOSED and %d is the file description\n", fileDescriptor);
}

int SerialPort::sendArray(string buffer, int len) {
	int n=write(fileDescriptor, buffer.c_str(), len);
	return n;
}

int SerialPort::getArray (string buffer, int len)
{
	bytesToRead();
	int n=0;//read(fileDescriptor, buffer.c_str(), len);
	return n;
}

void SerialPort::clear()
{
	tcflush(fileDescriptor, TCIFLUSH);
	tcflush(fileDescriptor, TCOFLUSH);
}

int SerialPort::bytesToRead()
{
	int bytes=0;
	ioctl(fileDescriptor, FIONREAD, &bytes);

	return bytes;
}
