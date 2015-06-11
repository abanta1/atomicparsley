//==================================================================//
/*
   AtomicParsley - outputxml.cpp

   AtomicParsley is GPL software; you can freely distribute, 
   redistribute, modify & use under the terms of the GNU General
   Public License; either version 2 or its successor.

   AtomicParsley is distributed under the GPL "AS IS", without
   any warranty; without the implied warranty of merchantability
   or fitness for either an expressed or implied particular purpose.

   Please see the included GNU General Public License (GPL) for 
   your rights and further details; see the file COPYING. If you
   cannot, write to the Free Software Foundation, 59 Temple Place
   Suite 330, Boston, MA 02111-1307, USA.  Or www.fsf.org

   Copyright ©2012
   with contributions from others; see the CREDITS file
 */
//==================================================================//

#include "AtomicParsley.h"

void APar_PrintName(char *name) {    
    memset(twenty_byte_buffer, 0, sizeof(char)*20);
    isolat1ToUTF8((unsigned char*)twenty_byte_buffer, 10, (unsigned char*)name, 4);
    if (UnicodeOutputStatus == WIN32_UTF16) {            
        APar_fprintf_UTF8_data(twenty_byte_buffer);
    } else {        
        fprintf(stdout, "%s", twenty_byte_buffer);
    }
}


void APar_ExtractDataAtomXML(int this_atom_number,char *name,char *reverseDNSdomain=NULL, char *reverseDNSname=NULL) {
	if ( source_file != NULL ) {
		AtomicInfo* thisAtom = &parsedAtoms[this_atom_number];

		uint32_t min_atom_datasize = 12;
		uint32_t atom_header_size = 16;

		if (thisAtom->AtomicClassification == EXTENDED_ATOM) {
			if (thisAtom->uuid_style == UUID_DEPRECATED_FORM) {
				min_atom_datasize +=4;
				atom_header_size +=4;
			} else {
				min_atom_datasize = 36;
				atom_header_size = 36;
			}
		}

		if (thisAtom->AtomicLength > min_atom_datasize ) {
			char* data_payload = (char*)malloc( sizeof(char) * (thisAtom->AtomicLength - atom_header_size +1) );
			memset(data_payload, 0, sizeof(char) * (thisAtom->AtomicLength - atom_header_size +1) );

			APar_readX(data_payload, source_file, thisAtom->AtomicStart + atom_header_size, thisAtom->AtomicLength - atom_header_size);

			if (thisAtom->AtomicVerFlags == AtomFlags_Data_Text) {
				if (thisAtom->AtomicLength < (atom_header_size + 4) ) {
					//tvnn was showing up with 4 chars instead of 3; easier to null it out for now
					data_payload[thisAtom->AtomicLength - atom_header_size] = '\00';
				}

				fprintf(stdout,"    <atomString name=\"");
                APar_PrintName(name);
                fprintf(stdout,"\"");
				if (reverseDNSdomain!=NULL) {
					fprintf(stdout," reverseDNSdomain=\"%s\"",reverseDNSdomain);
				}
				if (reverseDNSname!=NULL) {
					fprintf(stdout," reverseDNSname=\"%s\"",reverseDNSname);
				}
				fprintf(stdout,"><![CDATA[");
				APar_fprintf_UTF8_data(data_payload);
				fprintf(stdout,"]]></atomString>\n");

			} else {
				if ( (memcmp(name, "trkn", 4) == 0) || (memcmp(name, "disk", 4) == 0) ) {
                    fprintf(stdout, "    <atomRange name=\"");
                    APar_PrintName(name);
					fprintf(stdout, "\" count=\"%hu\"",UInt16FromBigEndian(data_payload+2));
					if (UInt16FromBigEndian(data_payload+4) != 0) {
						fprintf(stdout, " max=\"%hu\"",UInt16FromBigEndian(data_payload+4));
					} 
					fprintf(stdout,"/>\n");    
				} else if (strncmp(name, "gnre", 4) == 0) {
                    fprintf(stdout, "    <atomNumber name=\"");
                    APar_PrintName(name);
					fprintf(stdout, "\" value=\"%hu\"/>\n",UInt16FromBigEndian(data_payload));
				} else if ( (strncmp(name, "purl", 4) == 0) || (strncmp(name, "egid", 4) == 0) ) {
                    fprintf(stdout, "    <atom name=\"");
                    APar_PrintName(name);
					fprintf(stdout,"\" value=\"%s\"/>\n", data_payload); 
				} else {
					if (thisAtom->AtomicVerFlags == AtomFlags_Data_UInt && (thisAtom->AtomicLength <= 20 || thisAtom->AtomicLength == 24) ) {
						uint8_t bytes_rep = (uint8_t) (thisAtom->AtomicLength-atom_header_size);

						switch(bytes_rep) {
							case 1 : {
								if ( (memcmp(name, "cpil", 4) == 0) || (memcmp(name, "pcst", 4) == 0) || 
								     (memcmp(name, "pgap", 4) == 0)) {
                                    fprintf(stdout, "    <atomBoolean name=\"");
                                    APar_PrintName(name);
									if (data_payload[0] == 1) {                                        
										fprintf(stdout, "\" value=\"true\"/>\n");
									} else {
										fprintf(stdout, "\" value=\"false\"/>\n");
									}                                    
								} else if (strncmp(name, "stik", 4) == 0) {
                                    fprintf(stdout, "    <atomNumber name=\"");
                                    APar_PrintName(name);
									fprintf(stdout, "\" value=\"%hhu\"/>\n",data_payload[0]);
								} else if (strncmp(name, "rtng", 4) == 0) { //okay, this is definitely an 8-bit number
                                    fprintf(stdout, "    <atomNumber name=\"");
                                    APar_PrintName(name);
									fprintf(stdout, "\" value=\"%hhu\"/>\n",data_payload[0]);
								} else {
                                    fprintf(stdout, "    <atomNumber name=\"");
                                    APar_PrintName(name);
									fprintf(stdout, "\" value=\"%hhu\"/>\n",data_payload[0]);
								}
								break;
							}
							case 2 : { //tmpo
                                fprintf(stdout, "    <atomNumber name=\"");
                                APar_PrintName(name);
								fprintf(stdout, "\""); 
								if (reverseDNSdomain!=NULL) {
									fprintf(stdout," reverseDNSdomain=\"%s\"",reverseDNSdomain);
								}
								if (reverseDNSname!=NULL) {
									fprintf(stdout," reverseDNSname=\"%s\"",reverseDNSname);
								}
								fprintf(stdout," value=\"%hu\"/>\n",UInt16FromBigEndian(data_payload) );
								break;
							}
							case 4 : { //tves, tvsn
                                fprintf(stdout, "    <atomNumber name=\"");
                                APar_PrintName(name);
                                fprintf(stdout, "\"");
								if (reverseDNSdomain!=NULL) {
									fprintf(stdout," reverseDNSdomain=\"%s\"",reverseDNSdomain);
								}
								if (reverseDNSname!=NULL) {
									fprintf(stdout," reverseDNSname=\"%s\"",reverseDNSname);
								}
								fprintf(stdout," value=\"%u\"/>\n",UInt32FromBigEndian(data_payload) );
								break;
							}
							case 8 : {
                                fprintf(stdout, "    <atomNumber name=\"");
                                APar_PrintName(name);
                                fprintf(stdout, "\"");
								if (reverseDNSdomain!=NULL) {
									fprintf(stdout," reverseDNSdomain=\"%s\"",reverseDNSdomain);
								}
								if (reverseDNSname!=NULL) {
									fprintf(stdout," reverseDNSname=\"%s\"",reverseDNSname);
								}
								fprintf(stdout," value=\"%" PRIu64 "\"/>\n",UInt64FromBigEndian(data_payload) );
								break;
							}
						}

					} 
				}

				free(data_payload);
				data_payload = NULL;
			}
		}
	}
	return;
}

void APar_OutputXML() {
	short artwork_count=0;

	AtomicInfo *ilstAtom = APar_FindAtom("moov.udta.meta.ilst", false, SIMPLE_ATOM, 0);
	if (ilstAtom == NULL) {
		return;
	}
	fprintf(stdout,"<?xml version=\"1.0\"?>\n");
	fprintf(stdout,"<AtomicParsley>\n");
	fprintf(stdout,"  <atoms>\n");
	for (int i=ilstAtom->AtomicNumber; i < atom_number; i++) { 
		AtomicInfo* thisAtom = &parsedAtoms[i];
		if ( strncmp(thisAtom->AtomicName, "data", 4) == 0) { //thisAtom->AtomicClassification == VERSIONED_ATOM) {

			AtomicInfo* parent = &parsedAtoms[ APar_FindParentAtom(i, thisAtom->AtomicLevel) ];

			if ( (thisAtom->AtomicVerFlags == AtomFlags_Data_Binary ||
						thisAtom->AtomicVerFlags == AtomFlags_Data_Text || 
						thisAtom->AtomicVerFlags == AtomFlags_Data_UInt)) {
				if (strncmp(parent->AtomicName, "----", 4) == 0) {
					if (memcmp(parsedAtoms[parent->AtomicNumber+2].AtomicName, "name", 4) == 0) {
						APar_ExtractDataAtomXML(i,parent->AtomicName,parsedAtoms[parent->AtomicNumber+1].ReverseDNSdomain,parsedAtoms[parent->AtomicNumber+2].ReverseDNSname);
					}

				} else if (memcmp(parent->AtomicName, "covr", 4) == 0) { //libmp4v2 doesn't properly set artwork with the right flags (its all 0x00)
					artwork_count++;
				} else {
					APar_ExtractDataAtomXML(i,parent->AtomicName);
				}

			} else if (memcmp(parent->AtomicName, "covr", 4) == 0) {
				artwork_count++;
				//APar_ExtractAAC_Artwork(thisAtom->AtomicNumber, NULL, artwork_count);
			}
			if ( thisAtom->AtomicLength <= 12 ) {
				fprintf(stdout, "\n"); // (corrupted atom); libmp4v2 touching a file with copyright
			}
		}
		}

		if (artwork_count != 0) {
			for (int i=0;i<artwork_count;i++) {
				fprintf(stdout,"    <atomArtwork name=\"covr\"/>");
			}
		}
		fprintf(stdout,"  </atoms>\n");
		fprintf(stdout,"</AtomicParsley>\n");
		return;
	}
