# Introduction #

Comm library is meant for structured communication implementation on host and client sides. It doesn't provide any specific communication interface implementation. It is a way to design it. Read about the background on [accompanying post (it has pretty pictures)](http://searchingforbit.blogspot.com/2014/07/communication-with-embedded-devices.html).

# Details #

Library assumes query - response master - slave communication.

![https://lh4.googleusercontent.com/-pW4F5J5UAcc/U8p7coWD9YI/AAAAAAAAAMo/HzCQqIQ89bA/s746/CommFlow.jpg](https://lh4.googleusercontent.com/-pW4F5J5UAcc/U8p7coWD9YI/AAAAAAAAAMo/HzCQqIQ89bA/s746/CommFlow.jpg)

First implementation is made in C due to targeting embedded devices. Two major sets of functions are important:
  * `CommStack_X` functions
  * `CommInterface_X` functions

Functions have "this pointer" style for first parameter. C++ wrapper is possible and planed.

# CommStack interface #

This set of functions is called from application to communicate data. It's implemented in library. Library then calls `CommInterface_X` functions. For single instance of comm interface an object of type `SCommStack` must exist which is placed as "this pointer" parameter to `CommStack_X` functions.

```
struct _SCommStack;
typedef struct _SCommStack SCommStack;

typedef SCommStack * HCOMMSTACK;

struct _SCommStack
{
  SCommLayer *m_psCommLayer;      ///< Pointer to array of comm layers. Lowest layer (physical) at index 0.
  COMMCOUNT m_cntNumberOfLayers;  ///< Number of layers in array.
  unsigned char m_byFlags;        ///< Of ECommStackFlags.
  unsigned char m_byLastError;
};
```

Object is initialized in `CommStack_Init` call. Comm layers get connected in this call - after successful return stack is ready for communication.

```
HCOMMSTACK CommStack_Init(unsigned char byFlags, SCommStack * psCommStack, SCommLayer *psCommLayers, COMMCOUNT cntLayerCount);
void CommStack_PacketStart(HCOMMSTACK hCommStack);
void CommStack_Send(HCOMMSTACK hCommStack, const unsigned char *pbyData, COMMCOUNT cntByteCount);
void CommStack_PacketEnd(HCOMMSTACK hCommStack);
ECommStatus CommStack_TransmitProcess(HCOMMSTACK hCommStack);
ECommStatus CommStack_ReceiveProcess(HCOMMSTACK hCommStack, COMMCOUNT *pcntNumBytes, unsigned char **ppbyReceivedData);
COMMCOUNT CommStack_GetBufferSize(HCOMMSTACK hCommStack);
void CommStack_Disconnect(HCOMMSTACK hCommStack);

```

# CommLayer interface #

This set of functions is not called directly from application. Interface is meant to be implemented for each communication type, e.g. serial. `SCommLayer` type object represents single layer.
```
struct _SCommLayer;
typedef struct _SCommLayer SCommLayer;

struct _SCommLayer
{
  void *m_pLayerInstance;                     ///< "THIS" pointer.
  const ICommLayer *m_piCommLayer;            ///< Pointer to comm interface implementation.
  SCommLayer *m_psUpperLayer;                 ///< Pointer to upper comm layer.
  SCommLayer *m_psLowerLayer;                 ///< Pointer to lower comm layer.
  HCOMMSTACK m_hCommStack;                    ///< Comm stack handle to which layer belongs.
};
```
Since C is not objective language the type extension in terms of deriving is faked with `void *m_pLayerInstance` member where implementation can store own type object. It must do so in custom init function together with providing an address of `ICommLayer` instance which holds function pointers to functions implementing that layer.

```
static void myCommLayer_Init(SCommLayer *psCommLayer)
{
  SMyCommLayerType *psThis = (SMyCommLayerType *)psCommLayer->m_pLayerInstance;
  psThis->m_dwReceivedByteCount= 0;
  psThis->m_dwSendByteCount = 0;
}

static COMMCOUNT myCommLayer_CommGetBufferSize(SCommLayer *psCommLayer, COMMCOUNT cntLowerLayerBufferSize)
{
  SMyCommLayerType *psThis = (SMyCommLayerType *)psCommLayer->m_pLayerInstance;
  return psThis->m_dwBufferSizeInBytes;
}

static void myCommLayer_PacketStart(SCommLayer *psCommLayer)
{
  SMyCommLayerType *psThis = (SMyCommLayerType *)psCommLayer->m_pLayerInstance;
// impl
//   .
//   .
//   .
}

// other impl functions
//   .
//   .
//   .

static const ICommLayer sc_iMyCommLayerImpl =
{
  COMM_EVENT_NULL,                   // ClientInit
  myCommLayer_HostInit,              // HostInit
  myCommLayer_CommGetBufferSize,     // CommGetBufferSize
  myCommLayer_PacketStart,           // PacketStart
  myCommLayer_Send,                  // Send
// pointers to other impl functions
//   .
//   .
//   .
};

void MyCommLayer_Init(SCommLayer *psCommLayer, SMyCommLayerType *psMyCommLayerInstance, void * pBuffer, unsigned long dwBufferSizeInBytes)
{
  psCommLayer->m_pLayerInstance = psMyCommLayerInstance;
  psCommLayer->m_piCommLayer = &sc_iMyCommLayerImpl;
  psMyCommLayerInstance->m_pbyBuffer = pBuffer;
  psMyCommLayerInstance->m_dwBufferSizeInBytes = dwBufferSizeInBytes;
}
```