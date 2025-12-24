////////////////////////////////////////////////////////////////////////
//
// HierarchicalKeyFrameSAppl.h
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
//
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this
// software module or modifications thereof for use in hardware or
// software products claiming conformance to the MPEG-7 standard.
// Those intending to use this software module in hardware or software
// products are advised that its use may infringe existing patents.
// The original developer of this software module and his/her company,
// the subsequent editors and their companies, and ISO/IEC have no
// liability for use of this software module or modifications thereof
// in an application. No license to this software is granted for non
// MPEG-7 conforming products.
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module
// to a third party and to inhibit third parties from using the software
// module for non MPEG-7 conforming products.
//
// Copyright (c) 2000
//
// This copyright notice must be included in all copies or derivative
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#ifndef __HIERARCHICAL_KEY_FRAME_SERVER__
#define __HIERARCHICAL_KEY_FRAME_SERVER__
#include "BasicArchitecture/XMInterfaces.h"                                   /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                                      /* Naming Convention */
#include "Media/Media.h"


namespace XM
{

//=============================================================================
class HierarchicalKeyFrameServer: public ServerApplication
{
public:
        HierarchicalKeyFrameServer();
        virtual ~HierarchicalKeyFrameServer();

        virtual unsigned long Open();
        virtual unsigned long Start(char *ListFile, char *Bitstream);
        virtual unsigned long Stop();
        virtual unsigned long Close();

        virtual const UUID& GetObjectID(void);
        virtual const char *GetName(void);

        // access is allowed only by class factories for this
        // object.  This avoids having to duplicate the
        // ID definition in multiple locations.  In the future, we may
        // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE
        // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
        static const UUID myID;
        static const char * myName;
private:
        // maybe this class could contain the quantizer descriptor directly?
};
};
#endif //__HIERARCHICAL_KEY_FRAME_SERVER__

