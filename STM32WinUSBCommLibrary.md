# Communication flow chart #
![https://found-bits.googlecode.com/svn/wiki/WinUSBCommFlow.png](https://found-bits.googlecode.com/svn/wiki/WinUSBCommFlow.png)

# Device #
In firmware the library resides above user's USB driver implementation and below user'application code. See the comments in [WinUSBComm.h](https://code.google.com/p/found-bits/source/browse/trunk/Sources/Projects/Libs/WinUSBComm/WinUSBComm.h).
### USB driver ###
The USB implementation must call
```
void WinUSBComm_In();
```
after USB IN data packet was successfully sent and
```
void WinUSBComm_Out();
```
after USB OUT data packet was sent from host.
During these calls USB implementation will receive calls to:
```
void WinUSBComm_SetOutEndpointReady();
void WinUSBComm_ErrorOnOutEndpoint();
uint32_t WinUSBComm_GetNumBytesToReadFromOutEndpoint();
uint32_t WinUSBComm_ReadFromOutEndpoint(uint8_t *pbyData);
void WinUSBComm_SetInEndpointReady();
void WinUSBComm_ErrorOnInEndpoint();
void WinUSBComm_WriteToInEndpoint(uint8_t *pbyData, uint32_t dwByteCount);
```
In Set...Ready calls the state on endpoint must change to READY. In Error... calls the state on endpoint must change to STALL. Write and Read calls must transfer data to/from endpoints buffers.
USB implementation will also receive calls to:
```
uint32_t WinUSBComm_GetOutEndpointBufferSize();
uint32_t WinUSBComm_GetInEndpointBufferSize();
```
which must return endpoint buffer sizes.
The USB implementation must call
```
uint16_t WinUSBComm_Control(uint8_t byRequest, uint8_t **ppData);
```
when receives control requests for WinUSBComm interface. Function returns the length of data and pointer to data associated with request ID. It also changes the internal state of the WinUSBComm accordingly. The control requests are made up for library and defined in [WinUSBCommShared.h](https://code.google.com/p/found-bits/source/browse/trunk/Sources/Projects/Libs/WinUSBComm/WinUSBCommShared.h). The file is shared between host and device implementation.
### Application ###
The WinUSBComm must be polled constantly with calls to:
```
void WinUSBComm_ProcessNewData();
```
This function will call back the
```
void WinUSBComm_OnNewData();
```
if new data packet was received.
This is not single USB data packet. This is a larger packet received with more USB data packets. The max size of this packet is the WinUSBComm buffer size. It can be changed by user. Library has a buffer size request control through which the host learns about the max buffer size.
When in callback handler the user calls to
```
uint32_t WinUSBComm_GetAvailableByteCount();
```
to get how many bytes are still available to read and
```
uint8_t * WinUSBComm_ReadData(uint32_t dwLength);
void WinUSBComm_WriteData(uint8_t *pbyData, uint32_t dwLength);
```
To receive and send data.
The WinUSBComm has no buffer overflow mechanism. It must be guaranteed by design that a global query-response cycle won't use more than max buffer size per query. Larger data sizes can be achieved with a custom mechanism splitting what would have been a single cycle to multiple cycles. This mechanism can use the max buffer size information from the control request at the host side.
# Host #
_TODO_