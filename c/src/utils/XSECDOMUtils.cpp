/*
 * The Apache Software License, Version 1.1
 *
 *
 * Copyright (c) 2002-2003 The Apache Software Foundation.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:  
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "<WebSig>" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written 
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation and was
 * originally based on software copyright (c) 2001, Institute for
 * Data Communications Systems, <http://www.nue.et-inf.uni-siegen.de/>.
 * The development of this software was partly funded by the European 
 * Commission in the <WebSig> project in the ISIS Programme. 
 * For more information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

/*
 * XSEC
 *
 * XSECDOMUtils:= Utilities to manipulate DOM within XML-SECURITY
 *
 * Author(s): Berin Lautenbach
 *
 * $ID$
 *
 * $LOG$
 *
 */

// XSEC

#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>

// Xerces

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Utilities to manipulate DSIG namespaces
// --------------------------------------------------------------------------------

const XMLCh * getDSIGLocalName(const DOMNode *node) {

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIDSIG))
		return NULL; //DOMString("");
	else
		return node->getLocalName();

}

const XMLCh * getECLocalName(const DOMNode * node) {

	// Exclusive Canonicalisation namespace
	// Probably should have a generic function

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIEC))
		return NULL;
	else
		return node->getLocalName();

}

const XMLCh * getXPFLocalName(const DOMNode * node) {

	// XPath Filter namespace

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIXPF))
		return NULL;
	else
		return node->getLocalName();

}

const XMLCh DSIG_EXPORT * getXENCLocalName(const DOMNode *node) {

	// XML Encryption namespace node

	if (!strEquals(node->getNamespaceURI(), DSIGConstants::s_unicodeStrURIXENC))
		return NULL;
	else
		return node->getLocalName();

}


// --------------------------------------------------------------------------------
//           Find a nominated DSIG node in a document
// --------------------------------------------------------------------------------

DOMNode *findDSIGNode(DOMNode *n, const char * nodeName) {

	const XMLCh * name = getDSIGLocalName(n);

	if (strEquals(name, nodeName)) {

		return n;

	}

	DOMNode *child = n->getFirstChild();

	while (child != NULL) {

		DOMNode *ret = findDSIGNode(child, nodeName);
		if (ret != NULL)
			return ret;
		child = child->getNextSibling();

	}

	return child;

}

// --------------------------------------------------------------------------------
//           Find a nominated XENC node in a document
// --------------------------------------------------------------------------------

DOMNode *findXENCNode(DOMNode *n, const char * nodeName) {

	const XMLCh * name = getXENCLocalName(n);

	if (strEquals(name, nodeName)) {

		return n;

	}

	DOMNode *child = n->getFirstChild();

	while (child != NULL) {

		DOMNode *ret = findXENCNode(child, nodeName);
		if (ret != NULL)
			return ret;
		child = child->getNextSibling();

	}

	return child;

}

// --------------------------------------------------------------------------------
//           Find particular type of node child
// --------------------------------------------------------------------------------

DOMNode *findFirstChildOfType(DOMNode *n, DOMNode::NodeType t) {

	DOMNode *c;

	if (n == NULL) 
		return n;

	c = n->getFirstChild();

	while (c != NULL && c->getNodeType() != t)
		c = c->getNextSibling();

	return c;

}

DOMNode * findNextChildOfType(DOMNode *n, DOMNode::NodeType t) {

	DOMNode * s = n;

	if (s == NULL)
		return s;

	do {
		s = s->getNextSibling();
	} while (s != NULL && s->getNodeType() != t);

	return s;

}

// --------------------------------------------------------------------------------
//           Make a QName
// --------------------------------------------------------------------------------

safeBuffer &makeQName(safeBuffer & qname, safeBuffer &prefix, const char * localName) {

	if (prefix[0] == '\0') {
		qname = localName;
	}
	else {
		qname = prefix;
		qname.sbStrcatIn(":");
		qname.sbStrcatIn(localName);
	}

	return qname;

}
safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const char * localName) {

	if (prefix == NULL || prefix[0] == 0) {
		qname.sbTranscodeIn(localName);
	}
	else {
		qname.sbXMLChIn(prefix);
		qname.sbXMLChAppendCh(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
		qname.sbXMLChCat(localName);	// Will transcode
	}

	return qname;
}

safeBuffer &makeQName(safeBuffer & qname, const XMLCh *prefix, const XMLCh * localName) {

	if (prefix == NULL || prefix[0] == 0) {
		qname.sbXMLChIn(localName);
	}
	else {
		qname.sbXMLChIn(prefix);
		qname.sbXMLChAppendCh(XERCES_CPP_NAMESPACE_QUALIFIER chColon);
		qname.sbXMLChCat(localName);
	}

	return qname;
}

// --------------------------------------------------------------------------------
//           "Quick" Transcode (low performance)
// --------------------------------------------------------------------------------



XMLT::XMLT(const char * str) {

	mp_unicodeStr = XMLString::transcode(str);

}

XMLT::~XMLT (void) {

	delete[] mp_unicodeStr;

}

XMLCh * XMLT::getUnicodeStr(void) {
	
	return mp_unicodeStr;

}

// --------------------------------------------------------------------------------
//           Gather text from children
// --------------------------------------------------------------------------------

void gatherChildrenText(DOMNode * parent, safeBuffer &output) {

	DOMNode * c = parent->getFirstChild();

	output.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	while (c != NULL) {

		if (c->getNodeType() == DOMNode::TEXT_NODE)
			output.sbXMLChCat(c->getNodeValue());

		c = c->getNextSibling();

	}

}

// --------------------------------------------------------------------------------
//           Some UTF8 utilities
// --------------------------------------------------------------------------------

XMLCh * transcodeFromUTF8(const unsigned char * src) {

	// Take a UTF-8 buffer and transcode to UTF-16

	safeBuffer fullDest;
	fullDest.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);
	XMLCh outputBuf[2050];

	// Used to record byte sizes
	unsigned char charSizes[2050];

	// Grab a transcoder
	XMLTransService::Codes failReason;

#if defined(XSEC_XERCES_REQUIRES_MEMMGR)
	XMLTranscoder* t = 
		XMLPlatformUtils::fgTransService->makeNewTranscoderFor("UTF-8", 
															   failReason, 
															   2*1024, 
															   XMLPlatformUtils::fgMemoryManager);
#else
	XMLTranscoder* t = 
		XMLPlatformUtils::fgTransService->makeNewTranscoderFor("UTF-8", 
															   failReason, 
															   2*1024);
#endif
	Janitor<XMLTranscoder> j_t(t);

	// Need to loop through, 2K at a time
	unsigned int bytesEaten;
	unsigned int totalBytesEaten = 0;
	unsigned int bytesToEat = XMLString::stringLen((char *) src);

	while (totalBytesEaten < bytesToEat) {

		int toEat = (bytesToEat > 2048 ? 2048 : bytesToEat);

		t->transcodeFrom(&src[totalBytesEaten], 
						toEat, 
						outputBuf, 
						2048, 
						bytesEaten, 
						charSizes);

		outputBuf[bytesEaten] = chNull;
		fullDest.sbXMLChCat(outputBuf);
		totalBytesEaten += bytesEaten;
	}

	// Dup and output
	return XMLString::replicate(fullDest.rawXMLChBuffer());

}

// --------------------------------------------------------------------------------
//           String decode/encode
// --------------------------------------------------------------------------------

/*
 * Distinguished names have a particular encoding that needs to be performed prior
 * to enclusion in the DOM
 */

XMLCh * encodeDName(const XMLCh * toEncode) {

	XERCES_CPP_NAMESPACE_USE;

	safeBuffer result;

	static XMLCh s_strEncodedSpace[] = {
		chBackSlash,
		chDigit_2,
		chDigit_0,
		chNull
	};

	result.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	if (toEncode == NULL) {
		return NULL;
	}

	
	// Find where the trailing whitespace starts
	const XMLCh * ws = &toEncode[XMLString::stringLen(toEncode)];
	
	*ws--;
	while (ws != toEncode && 
		(*ws == '\t' || *ws == '\r' || *ws ==' ' || *ws == '\n'))
		*ws--;

	// Set to first white space character, if we didn't get back to the start
	if (toEncode != ws)
		*ws++;

	// Now run through each character and encode if necessary

	const XMLCh * i = toEncode;

	if (*i == chPound) {
		// "#" Characters escaped at the start of a string
		result.sbXMLChAppendCh(chBackSlash);
	}

	while (*i != chNull && i != ws) {

		if (*i <= 0x09) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_0);
			result.sbXMLChAppendCh(chDigit_0 + *i);
		}
		else if (*i <= 0x0f) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_0);
			result.sbXMLChAppendCh(chLatin_A + *i);
		}
		else if (*i <= 0x19) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_1);
			result.sbXMLChAppendCh(chDigit_0 + *i);
		}
		else if (*i <= 0x1f) {
			result.sbXMLChAppendCh(chBackSlash);
			result.sbXMLChAppendCh(chDigit_1);
			result.sbXMLChAppendCh(chLatin_A + *i);
		}

		else if (*i == chComma) {

			// Determine if this is an RDN separator
			const XMLCh *j = i;
			*j++;
			while (*j != chComma && *j != chEqual && *j != chNull)
				*j++;

			if (*j != chEqual)
				result.sbXMLChAppendCh(chBackSlash);

			result.sbXMLChAppendCh(*i);

		}

		else {
			
			if (*i == chPlus ||
				*i == chDoubleQuote ||
				*i == chBackSlash ||
				*i == chOpenAngle ||
				*i == chCloseAngle ||
				*i == chSemiColon) {

				result.sbXMLChAppendCh(chBackSlash);
			}

			result.sbXMLChAppendCh(*i);

		}

		*i++;

	}

	// Now encode trailing white space
	while (*i != chNull) {

		if (*i == ' ')
			result.sbXMLChCat(s_strEncodedSpace);
		else
			result.sbXMLChAppendCh(*i);

		*i++;

	}

	return XMLString::replicate(result.rawXMLChBuffer());

}

XMLCh * decodeDName(const XMLCh * toDecode) {

	// Take an encoded name and decode to a normal XMLCh string

	XERCES_CPP_NAMESPACE_USE;

	safeBuffer result;

	result.sbXMLChIn(DSIGConstants::s_unicodeStrEmpty);

	if (toDecode == NULL) {
		return NULL;
	}

	const XMLCh * i = toDecode;

	if (*i == chBackSlash && i[1] == chPound) {

		result.sbXMLChAppendCh(chPound);
		*i++;
		*i++;

	}

	while (*i != chNull) {

		if (*i == chBackSlash) {

			*i++;
			
			if (*i == chDigit_0) {

				*i++;

				if (*i >= chDigit_0 && *i <= chDigit_9) {
					result.sbXMLChAppendCh(*i - chDigit_0);
				}
				else if (*i >= chLatin_A && *i <= chLatin_F) {
					result.sbXMLChAppendCh(10 + *i - chLatin_A);
				}
				else if (*i >= chLatin_a && *i <= chLatin_f) {
					result.sbXMLChAppendCh(10 + *i - chLatin_a);
				}
				else {
					throw XSECException(XSECException::DNameDecodeError,
						"Unexpected escaped character in Distinguished name");
				}
			}

			else if (*i == chDigit_1) {

				*i++;

				if (*i >= chDigit_0 && *i <= chDigit_9) {
					result.sbXMLChAppendCh(16 + *i - chDigit_0);
				}
				else if (*i >= chLatin_A && *i <= chLatin_F) {
					result.sbXMLChAppendCh(26 + *i - chLatin_A);
				}
				else if (*i >= chLatin_a && *i <= chLatin_f) {
					result.sbXMLChAppendCh(26 + *i - chLatin_a);
				}
				else {
					throw XSECException(XSECException::DNameDecodeError,
						"Unexpected escaped character in Distinguished name");
				}
			}

			else if (*i == chDigit_2) {

				*i++;

				if (*i == '0') {
					result.sbXMLChAppendCh(' ');
				}

				else {
					throw XSECException(XSECException::DNameDecodeError,
						"Unexpected escaped character in Distinguished name");
				}

			}

			else if (*i == chComma ||
					 *i == chPlus ||
					 *i == chDoubleQuote ||
					 *i == chBackSlash ||
					 *i == chOpenAngle ||
					 *i == chCloseAngle ||
					 *i == chSemiColon) {

				result.sbXMLChAppendCh(*i);
			}

			else {

				throw XSECException(XSECException::DNameDecodeError,
					"Unexpected escaped character in Distinguished name");

			}

			*i++;

		}

		else {

			result.sbXMLChAppendCh(*i++);

		}

	}

	return XMLString::replicate(result.rawXMLChBuffer());

}
