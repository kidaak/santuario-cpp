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
 * WinCAPICryptoKeyHMAC := Windows HMAC keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id$
 *
 */

#include <xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.hpp>
#include <xsec/framework/XSECError.hpp>
#include <xsec/enc/XSECCryptoException.hpp>

WinCAPICryptoKeyHMAC::WinCAPICryptoKeyHMAC(HCRYPTPROV prov) :m_keyBuf("") {

	m_keyBuf.isSensitive();
	m_keyLen = 0;
	m_k = 0;
	m_p = prov;

};

void WinCAPICryptoKeyHMAC::setKey(unsigned char * inBuf, unsigned int inLength) {

	m_keyBuf.sbMemcpyIn(inBuf, inLength);
	m_keyLen = inLength;

}

unsigned int WinCAPICryptoKeyHMAC::getKey(safeBuffer &outBuf) {

	outBuf = m_keyBuf;
	return m_keyLen;

}

XSECCryptoKey * WinCAPICryptoKeyHMAC::clone() {

	WinCAPICryptoKeyHMAC * ret;

	XSECnew(ret, WinCAPICryptoKeyHMAC(m_p));

	ret->m_keyBuf = m_keyBuf;
	ret->m_keyLen = m_keyLen;

	if (m_k != 0) {
#if (_WIN32_WINNT > 0x0400)
		if (CryptDuplicateKey(m_k,
			 				  0,
							  0,
							  &(ret->m_k)) == 0 ) {

			throw XSECCryptoException(XSECCryptoException::MDError,
				"WinCAPI:KeyHMAC Error attempting to clone key parameters");

		}
#else
		throw XSECCryptoException(XSECCryptoException::MDError,
			"Unable to clone keys in Windows NT 4.0 and below");
#endif
	}
	else
		ret->m_k = 0;

	return ret;

}

// --------------------------------------------------------------------------------
//           Windows Specific Keys
// --------------------------------------------------------------------------------

void WinCAPICryptoKeyHMAC::setWinKey(HCRYPTKEY k) {

	if (m_k != 0) {

		CryptDestroyKey(m_k);

	}

	m_k = k;

}

HCRYPTKEY WinCAPICryptoKeyHMAC::getWinKey(void) {

	return m_k;

}

HCRYPTPROV WinCAPICryptoKeyHMAC::getWinKeyProv(void) {

	return m_p;

}