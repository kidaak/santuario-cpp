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
 * XSECPlatformUtils:= To support the platform we run in
 *
 * Author(s): Berin Lautenbach
 *
 * $Id$
 *					 
 */

#ifndef XSECPLATFORMUTILS_INCLUDE
#define XSECPLATFORMUTILS_INCLUDE

// XSEC

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/enc/XSECCryptoProvider.hpp>

class XSECAlgorithmMapper;
class XSECAlgorithmHandler;

#include <stdio.h>

/**
 * \brief High level library interface class.
 * @ingroup internal
 *
 * This class is used primarily to initialise the library and
 * communicate high level parameters that will be common to all
 * objects from the class in any given session.
 *
 * It is primarily a static class.
 */

class DSIG_EXPORT XSECPlatformUtils {

public :

	/**
	 * \brief Number of times initialise has been called 
	 *
	 * initCount can be read by any class or function to determine how
	 * many times the library has been initialised.
	 */

	static int initCount;

	/**
	 * \brief The main cryptographic provider
	 *
	 * This pointer can be used to determine the primary crypto
	 * provider registered in the library.
	 *
	 * Individual signatures can over-ride this default.
	 *
	 */

	static XSECCryptoProvider * g_cryptoProvider;

	/**
	 * \brief The global Algorithm Mapper
	 *
	 * The algorithm mapper is used to map algorithm type URI strings
	 * to algorithm implementations.  Note that this is a level of
	 * indirection above actual cryptographic algorithms.  For example:
	 *
	 * URI = http://www.w3.org/2001/04/xmlenc#tripledes-cbc
	 *
	 * is the URI for 3DES in CBC mode.  The mapper will return an
	 * algorithm handler that understands what this means in terms of
	 * IVs and how to call the XSECCryptoKey interface.  It then uses the
	 * cryptographic provider to actually perform the encryption.
	 *
	 * This allows applications to provide new algorithm types.  The
	 * mapper is used to map the type string to the means of doing the
	 * encryption, and a new XSECCryptoKey derivative can be provided
	 * to perform the actual crypo work.
	 *
	 * @note The provider should only be added to via the
	 * XSECPlatformUtils::addAlgorithmHandler() call.
	 *
	 * @see #addAlgorithmHandler()
	 */

	static const XSECAlgorithmMapper * g_algorithmMapper;

	/**
	 * \brief Initialise the library
	 *
	 * <b>Must</b> be called prior to using any functions in the library.
	 *
	 * Primarily sets up static variables used by all classes in the
	 * library.
	 *
	 * @param p A pointer to a XSECCryptoProvider object that the library 
	 * should use for cryptographic functions.  If p == NULL, the library
	 * will instantiate an OpenSSLCryptoProvider object.
	 */

	static void Initialise(XSECCryptoProvider * p = NULL);

	/**
	 * \brief Set a new crypto provider
	 * 
	 * Set the crypto provider to the value passed in.  Any current provider
	 * is deleted.
	 *
	 * @note This is not thread-safe.  It should be called prior to any real
	 * usage of the library.
	 *
	 * @param p A pointer to a XSECCryptoProvider object that the library 
	 * should use for cryptographic functions.  
	 * @note Ownership of the provider is passed to the library, which will
	 * delete it at Termination.
	 */

	static void SetCryptoProvider(XSECCryptoProvider * p);

	/**
	 * \brief Add a new algorithm Handler
	 *
	 * Application developers can extend the XSECAlgorithmHandler class to
	 * implement new cryptographic algorithms.  This will then allow the
	 * library to call the provided handler whenever trying to process a
	 * type it doesn't understand.
	 *
	 * Any handler previously registered for this URI will be overwritten,
	 * allowing callers to overwrite the handlers for default URIs.
	 *
	 * @see XSECAlgorithmHandler
	 * @note This is <b>not</b> thread safe.  Algorithm handlers should
	 * be added prior to any processing of signatures etc.
	 * @param uri Type URI that maps to this handler
	 * @param handler The handler to be used whenever this URI is seen by
	 * the library.
	 */

	static void registerAlgorithmHandler(const XMLCh * uri, const XSECAlgorithmHandler & handler);

	/**
	 * \brief Terminate
	 *
	 * Should be called prior to any program exist to allow the library
	 * to cleanly delete any memory associated with the library as a whole.
	 *
	 * @note Do not call this function while any xml-security-c object
	 * remain instantiated.  The results of doing so is undefined, and could
	 * cause bad results.
	 */

	static void Terminate(void);

};


#endif /* XSECPLATFORMUTILS_INCLUDE */
