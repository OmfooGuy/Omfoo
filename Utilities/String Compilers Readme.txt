These developer's utilities are part of the Omfoo Source Code Project.
You only need these of you are compiling your own version of Omfoo.dll.
They generate the lookup tables found in ReadOmf_StringTables.cpp.

CReadOmf_StringTables::m_aPropertyStringHashTable[];
CReadOmf_StringTables::m_aPropertyStringOffsets[];
CReadOmf_StringTables::m_aPropertyStringData[];

CReadOmf_StringTables::m_aDataTypeStringHashTable[];
CReadOmf_StringTables::m_aDataTypeStringOffsets[];
CReadOmf_StringTables::m_aDataTypeStringData[];

CReadOmf_StringTables::m_aDataKindStringHashTable[];
CReadOmf_StringTables::m_aDataKindStringOffsets[];
CReadOmf_StringTables::m_aDataKindStringData[];

CReadOmf_StringTables::m_aEffectIDStringHashTable[];
CReadOmf_StringTables::m_aEffectIDStringOffsets[];
CReadOmf_StringTables::m_aEffectIDStringData[];

They also generate the corresponding enumerated values found in ReadOmf_StringOrdinals.h.
See CReadOmf::OrdinalToPropertyID() and the COmfObject::OrdReadXXX() routines in OmfObject.h.

