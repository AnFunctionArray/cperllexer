typedef int SID, BYTE, DWORD;

typedef union _SE_SID {
    SID Sid;
    BYTE  Buffer[(sizeof(SID) - sizeof(DWORD) + ((15) * sizeof(DWORD)))];
} SE_SID, *PSE_SID;