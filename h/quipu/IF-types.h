/* automatically generated by pepsy 8.0 #14 (5825806cb830), do not edit! */

#ifndef	_module_IF_defined_
#define	_module_IF_defined_

#ifndef	PEPSY_VERSION
#define	PEPSY_VERSION		2
#endif

#ifndef	PEPYPATH
#include <isode/psap.h>
#include <isode/pepsy.h>
#include <isode/pepsy/UNIV-types.h>
#else
#include "psap.h"
#include "pepsy.h"
#include "UNIV-types.h"
#endif



extern modtyp	_ZIF_mod;
#define _ZAttributeValueIF	1
#define _ZAttributeTypeIF	0
#define _ZRDNSequenceIF	6
#define _ZNameIF	8
#define _ZAttributeValueAssertionIF	2
#define _ZRelativeDistinguishedNameIF	5
#define _ZD_AValuesIF	4
#define _ZAttributeIF	3
#define _ZDistinguishedNameIF	7

#ifndef	lint
#define encode_IF_AttributeType(pe, top, len, buffer, parm) \
    enc_f(_ZAttributeTypeIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_AttributeType(pe, top, len, buffer, parm) \
    dec_f(_ZAttributeTypeIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_AttributeType(pe, top, len, buffer, parm) \
    prnt_f(_ZAttributeTypeIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_AttributeType_P    _ZAttributeTypeIF, &_ZIF_mod

#define encode_IF_AttributeValue(pe, top, len, buffer, parm) \
    enc_f(_ZAttributeValueIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_AttributeValue(pe, top, len, buffer, parm) \
    dec_f(_ZAttributeValueIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_AttributeValue(pe, top, len, buffer, parm) \
    prnt_f(_ZAttributeValueIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_AttributeValue_P    _ZAttributeValueIF, &_ZIF_mod

#define encode_IF_AttributeValueAssertion(pe, top, len, buffer, parm) \
    enc_f(_ZAttributeValueAssertionIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_AttributeValueAssertion(pe, top, len, buffer, parm) \
    dec_f(_ZAttributeValueAssertionIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_AttributeValueAssertion(pe, top, len, buffer, parm) \
    prnt_f(_ZAttributeValueAssertionIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_AttributeValueAssertion_P    _ZAttributeValueAssertionIF, &_ZIF_mod

#define encode_IF_Attribute(pe, top, len, buffer, parm) \
    enc_f(_ZAttributeIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_Attribute(pe, top, len, buffer, parm) \
    dec_f(_ZAttributeIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_Attribute(pe, top, len, buffer, parm) \
    prnt_f(_ZAttributeIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_Attribute_P    _ZAttributeIF, &_ZIF_mod

#define encode_IF_D__AValues(pe, top, len, buffer, parm) \
    enc_f(_ZD_AValuesIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_D__AValues(pe, top, len, buffer, parm) \
    dec_f(_ZD_AValuesIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_D__AValues(pe, top, len, buffer, parm) \
    prnt_f(_ZD_AValuesIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_D__AValues_P    _ZD_AValuesIF, &_ZIF_mod

#define encode_IF_RelativeDistinguishedName(pe, top, len, buffer, parm) \
    enc_f(_ZRelativeDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_RelativeDistinguishedName(pe, top, len, buffer, parm) \
    dec_f(_ZRelativeDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_RelativeDistinguishedName(pe, top, len, buffer, parm) \
    prnt_f(_ZRelativeDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_RelativeDistinguishedName_P    _ZRelativeDistinguishedNameIF, &_ZIF_mod

#define encode_IF_RDNSequence(pe, top, len, buffer, parm) \
    enc_f(_ZRDNSequenceIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_RDNSequence(pe, top, len, buffer, parm) \
    dec_f(_ZRDNSequenceIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_RDNSequence(pe, top, len, buffer, parm) \
    prnt_f(_ZRDNSequenceIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_RDNSequence_P    _ZRDNSequenceIF, &_ZIF_mod

#define encode_IF_DistinguishedName(pe, top, len, buffer, parm) \
    enc_f(_ZDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_DistinguishedName(pe, top, len, buffer, parm) \
    dec_f(_ZDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_DistinguishedName(pe, top, len, buffer, parm) \
    prnt_f(_ZDistinguishedNameIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_DistinguishedName_P    _ZDistinguishedNameIF, &_ZIF_mod

#define encode_IF_Name(pe, top, len, buffer, parm) \
    enc_f(_ZNameIF, &_ZIF_mod, pe, top, len, buffer, (char *) parm)

#define decode_IF_Name(pe, top, len, buffer, parm) \
    dec_f(_ZNameIF, &_ZIF_mod, pe, top, len, buffer, (char **) parm)

#define print_IF_Name(pe, top, len, buffer, parm) \
    prnt_f(_ZNameIF, &_ZIF_mod, pe, top, len, buffer)
#define print_IF_Name_P    _ZNameIF, &_ZIF_mod


#endif   /* lint */
#include "if-cdefs.h"

#define	type_IF_AttributeType	OIDentifier

#define	type_IF_AttributeValue	PElement

#define	type_IF_DistinguishedName	type_IF_RDNSequence

#define	type_IF_Name	type_IF_RDNSequence

struct type_IF_AttributeValueAssertion {
    struct type_IF_AttributeType *element_IF_0;

    struct type_IF_AttributeValue *element_IF_1;
};
#define	free_IF_AttributeValueAssertion(parm)\
	 fre_obj((char *) parm, _ZIF_mod.md_dtab[_ZAttributeValueAssertionIF], &_ZIF_mod, 1)

struct type_IF_Attribute {
    struct type_IF_AttributeType *type;

    struct type_IF_D__AValues *values;
};
#define	free_IF_Attribute(parm)\
	 fre_obj((char *) parm, _ZIF_mod.md_dtab[_ZAttributeIF], &_ZIF_mod, 1)

struct type_IF_D__AValues {
        struct type_IF_AttributeValue *member_IF_0;

        struct type_IF_D__AValues *next;
};
#define	free_IF_D__AValues(parm)\
	 fre_obj((char *) parm, _ZIF_mod.md_dtab[_ZD_AValuesIF], &_ZIF_mod, 1)

struct type_IF_RelativeDistinguishedName {
        struct element_IF_2 {
            struct type_IF_AttributeType *element_IF_3;

            struct type_IF_AttributeValue *element_IF_4;
        } *member_IF_1;

        struct type_IF_RelativeDistinguishedName *next;
};
#define	free_IF_RelativeDistinguishedName(parm)\
	 fre_obj((char *) parm, _ZIF_mod.md_dtab[_ZRelativeDistinguishedNameIF], &_ZIF_mod, 1)

struct type_IF_RDNSequence {
        struct type_IF_RelativeDistinguishedName *element_IF_5;

        struct type_IF_RDNSequence *next;
};
#define	free_IF_RDNSequence(parm)\
	 fre_obj((char *) parm, _ZIF_mod.md_dtab[_ZRDNSequenceIF], &_ZIF_mod, 1)
#endif
