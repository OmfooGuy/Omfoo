// Indent=4, tab=4, column width=120, CR/LF, codepage=ASCII
// Original filename: Omfoo_Enumerated_Types.h
// Copyright (C) 2022 David Miller
// This file is part of the Omfoo API. You should have received a
// copy of the Omfoo end-user license agreement (EULA) along with it.
// Please see OMFOO_EULA.TXT or send inquiries to OmfooGuy@gmail.com.
// ---------------------------------------------------------------------------
#ifndef __OMFOO_ENUMERATED_TYPES_H__
#define __OMFOO_ENUMERATED_TYPES_H__

//	OMFI:ATTB:Kind, omfi:AttrKind
enum OMF_ATTRIBUTE_KIND {
	AK_NULL				= 0x0000,	// Unspecified type
	AK_INT32			= 0x0001,	// omfi:Int32
	AK_STRING			= 0x0002,	// omfi:String
	AK_OBJECT			= 0x0003,	// omfi:ObjRef
	AK_DATAVALUE		= 0x0004,	// omfi:DataValue or omfi:VarLenBytes
};

//	OMFI:CharacterSet, omfi:CharSetType
enum OMF_CHARSET_TYPE {
	CS_NULL				= 0x0000,
	CS_ASCII			= 0x0001,
};

//	OMFI:DIDD:FrameLayout, omfi:LayoutType
enum OMF_DIDD_LAYOUT_TYPE {
	LT_FULL_FRAME		= 0x0000,
	LT_SEPARATE_FIELDS	= 0x0001,
	LT_SINGLE_FIELD		= 0x0002,
	LT_MIXE_FIELDS		= 0x0003,
};

//	OMFI:CMOB:DefFadeType, omfi:FadeType
enum OMF_FADE_TYPE {
	FT_NONE				= 0x0000,
	FT_LINEAR_AMP		= 0x0001,
	FT_LINEAR_PWR		= 0x0002,
	FT_LINEAR_dB		= 0x0003,
};

//	OMFI:MOBJ:AppCode, omfi:Int32
//	OMFI:MOBJ:UsageCode, omfi:UsageCodeType
enum OMF_MOB_APP_CODE {
	AC_NULL				= 0x00000000,
	AC_PRECOMPUTE		= 0x00000001,
	AC_SUBCLIP			= 0x00000002,
	AC_EFFECT			= 0x00000003,
	AC_GROUP			= 0x00000004,
	AC_GROUPINDIRECT	= 0x00000005,
	AC_MOTION			= 0x00000006,
	AC_MASTER			= 0x00000007,
	AC_STUB				= 0x00000008,
	AC_PRECOMPUTE_FILE	= 0x00000009,
	AC_SYNTHETIC		= 0x0000000A,
	AC_META				= 0x0000000B,
	AC_TRIGGER			= 0x0000000C,
	AC_UNDOCUMENTED_13	= 0x0000000D,	// ???
	AC_PRECOMPUTE_SMOB	= 0x0000000E,
};

//	OMFI:MDES:MobKind, omfi:PhysicalMobType
enum OMF_PHYSICAL_MOB_TYPE {
	PT_NULL_MOB			= 0x0000,
	PT_FILE_MOB			= 0x0001,	// The MDES is the OMF1 equivalent of a MDFL
	PT_TAPE_MOB			= 0x0002,	// The MDES is the OMF1 equivalent of a MDTP
	PT_FILM_MOB			= 0x0003,	// The MDES is the OMF1 equivalent of a MDFM
	PT_NAGRA_MOB		= 0x0004,	// The MDES is the OMF1 equivalent of a MDNG
	PT_ORIGIN_MOB		= 0x0005,	// The OMFI:MDES:Locator property identifies the original file that was ingested.
};

// OMFI:CPNT:TrackKind, omfi:TrackType
enum OMF_TRACK_TYPE {
	TT_NULL				= 0x0000,
	TT_PICTURE			= 0x0001,
	TT_SOUND			= 0x0002,
	TT_TIMECODE			= 0x0003,
	TT_EDGECODE			= 0x0004,
	TT_USERATTR			= 0x0005,
	TT_EFFECTDATA		= 0x0006,
	TT_MARKER			= 0x0007,
	TT_STEREO_SND		= 0x0008,
	TT_CONTROL			= 0x0009,
	TT_TEXT				= 0x000A,
	TT_MIDI				= 0x000B,
	TT_META				= 0x000C,
};

// Here we map the OMF1 OMFI:CPNT:TrackKind to the OMF2 OMFI:DDEF:DataKindID.
// OMFI:CPNT:TrackKind, omfi:TrackType
// OMFI:DDEF:DataKindID, omfi:UniqueName
enum OMF_DATA_KIND {				// OMF2 (an ascii string)	OMF1 (a 16-bit unsigned integer)
									// OMFI:DDEF:DataKindID		OMFI:CPNT:TrackKind
	DK_NULL				= 0x0000,	// omfi:data:NoDatakind		TT_NULL
	DK_PICTURE			= 0x0001,	// omfi:data:Picture		TT_PICTURE
	DK_SOUND			= 0x0002,	// omfi:data:Sound			TT_SOUND
	DK_TIMECODE			= 0x0003,	// omfi:data:Timecode		TT_TIMECODE
	DK_EDGECODE			= 0x0004,	// omfi:data:Edgecode		TT_EDGECODE
	DK_USERATTR			= 0x0005,	// omfi:data:USERATTR		TT_USERATTR
	DK_EFFECTDATA		= 0x0006,	// omfi:data:EFFECTDATA		TT_EFFECTDATA
	DK_MARKER			= 0x0007,	// omfi:data:MARKER			TT_MARKER
	DK_STEREO_SND		= 0x0008,	// omfi:data:StereoSound	TT_STEREO_SND
	DK_CONTROL			= 0x0009,	// omfi:data:CONTROL		TT_CONTROL
	DK_STRING			= 0x000A,	// omfi:data:String			TT_TEXT
	DK_MIDI				= 0x000B,	// omfi:data:MIDI			TT_MIDI
	DK_META				= 0x000C,	// omfi:data:META			TT_META
	DK_RESERVED_000D	= 0x000D,	// Reserved for if/when we ever discover more undocumented OMF1 omfi:TrackTypes.
	DK_RESERVED_000E	= 0x000E,	// ...
	DK_RESERVED_000F	= 0x000F,	// ...

	DK_AUDIOEQBAND		= 0x0010,	// omfi:data:AudioEQBand
	DK_COLOR			= 0x0011,	// omfi:data:Color
	DK_COLORSPACE		= 0x0012,	// omfi:data:ColorSpace
	DK_DIRECTIONCODE	= 0x0013,	// omfi:data:DirectionCode
	DK_DISTANCE			= 0x0014,	// omfi:data:Distance
	DK_EFFECTGLOBALS	= 0x0015,	// omfi:data:EffectGlobals
	DK_EFFECTGLOBALSEXT	= 0x0016,	// omfi:data:EffectGlobalsExt
	DK_BOOLEAN			= 0x0017,	// omfi:data:Boolean
	DK_CHAR				= 0x0018,	// omfi:data:Char
	DK_INT8				= 0x0019,	// omfi:data:Int8
	DK_INT16			= 0x001A,	// omfi:data:Int16
	DK_INT32			= 0x001B,	// omfi:data:Int32
	DK_INT64			= 0x001C,	// omfi:data:Int64
	DK_UINT8			= 0x001D,	// omfi:data:UInt8
	DK_UINT16			= 0x001E,	// omfi:data:UInt16
	DK_UINT32			= 0x001F,	// omfi:data:UInt32
	DK_UINT64			= 0x0020,	// omfi:data:UInt64
	DK_KEYFRAME			= 0x0021,	// omfi:data:KeyFrame
	DK_MATTE			= 0x0022,	// omfi:data:Matte
	DK_PICTUREWITHMATTE	= 0x0023,	// omfi:data:PictureWithMatte
	DK_POINT			= 0x0024,	// omfi:data:Point
	DK_POLYNOMIAL		= 0x0025,	// omfi:data:Polynomial
	DK_RATIONAL			= 0x0026,	// omfi:data:Rational
	DK_USERPARAM		= 0x0027,	// omfi:data:UserParam
	DK_INVALID			= 0x0028,	// omfi:data:INVALID
};

// OMFI:EDEF:EffectID, omfi:UniqueName
enum OMF_EFFECT_KIND {
	EK_UNKNOWN						= 0x0000,	// omfi:effect:Unknown
	EK_MONO_AUDIO_GAIN				= 0x0001,	// omfi:effect:MonoAudioGain
	EK_MONO_AUDIO_MIXDOWN			= 0x0002,	// omfi:effect:MonoAudioMixdown
	EK_MONO_AUDIO_PAN				= 0x0003,	// omfi:effect:MonoAudioPan
	EK_SIMPLE_MONO_AUDIO_DISSOLVE	= 0x0004,	// omfi:effect:SimpleMonoAudioDissolve
	EK_SIMPLE_VIDEO_DISSOLVE		= 0x0005,	// omfi:effect:SimpleVideoDissolve
	EK_SMPTE_VIDEO_WIPE				= 0x0006,	// omfi:effect:SMPTEVideoWipe
	EK_VIDEO_FADE_TO_BLACK			= 0x0007,	// omfi:effect:VideoFadeToBlack
	EK_VIDEO_FRAME_MASK				= 0x0008,	// omfi:effect:VideoFrameMask
	EK_VIDEO_REPEAT					= 0x0009,	// omfi:effect:VideoRepeat
	EK_VIDEO_SPEED_CONTROL			= 0x000A,	// omfi:effect:VideoSpeedControl
};

// Bit flags returned by IOmfooReader::GetInfoBits().
enum READER_INFO_BIT_FLAGS {
	RIB_BENTO_VERSION2		= 0x00000001,	// set if the Bento layer is revision 1.05, clear if it is revision 1.04
	RIB_BENTO_BIG_ENDIAN	= 0x00000002,	// set if the Bento layer is big-endian, clear if it is little-endian
	RIB_BENTO_EXTRA_LABEL	= 0x00000004,	// set if the Bento label includes a 64-bit extension
	RIB_BENTO_RESERVED		= 0x000000F8,	// reserved for future use

	RIB_OMF_VERSION2		= 0x00000100,	// set if the file is OMF version 2.x, or clear if it is OMF version 1.x.
	RIB_OMF_BIG_ENDIAN		= 0x00000200,	// set if the file's OMF layer is big-endian
	RIB_OMF_RESERVED		= 0x0000FC00,	// reserved for future use

//	RIB_				// set if Timestamp bug

//	RIB_MDAT_BAD_MOBIDS	= 0xHHHHHHHH,		// set if the file contains MDATs with nonunique (duplicate) mobIDs.
//	RIB_MOBJ_BAD_MOBIDS	= 0xHHHHHHHH,		// set if the file contains MOBJs with nonunique (duplicate) mobIDs.

	RIB_TOUCHED_BY_WIN		= 0x00010000,	// set if the file was created or modified by a Windows-based program
	RIB_TOUCHED_BY_MAC		= 0x00020000,	// set if the file was created or modified by a Macintosh-based program
};

#endif	// __OMFOO_ENUMERATED_TYPES_H__
