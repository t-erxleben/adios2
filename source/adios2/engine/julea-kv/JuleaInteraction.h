/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * JULEA engine using the JULEA storage framework to handle lower I/O.
 *
 *  Created on: Aug 01, 2019
 *      Author: Kira Duwe duwe@informatik.uni-hamburg.de
 */

#ifndef ADIOS2_ENGINE_JULEAINTERACTION_H_
#define ADIOS2_ENGINE_JULEAINTERACTION_H_

#include "JuleaMetadata.h"
#include "JuleaKVWriter.h"

// #include <julea.h>

namespace adios2
{
namespace core
{
namespace engine
{

template <class T>
void PutVariableDataToJulea(Variable<T> &variable, const T *data, const char *nameSpace);

template <class T>
void PutVariableMetadataToJulea(Variable<T> &variable, const bson_t *bsonMetadata, const char *nameSpace);


template <class T>
void PutAttributeDataToJulea(Attribute<T> &attribute, const T *data, const char *nameSpace);

template <class T>
void PutAttributeMetadataToJulea(Attribute<T> &attribute, const bson_t *bsonMetadata, const char *nameSpace);


#define declare_template_instantiation(T)                                      \
    extern template void PutVariableDataToJulea(Variable<T> &variable, const T *data, const char *nameSpace); \
    extern template void PutVariableMetadataToJulea(Variable<T> &variable, const bson_t *bsonMetadata, const char *nameSpace);     \
    extern template void PutAttributeDataToJulea(Attribute<T> &attribute, const T *data, const char *nameSpace);\
    extern template void PutAttributeDataToJulea(Attribute<T> &attribute, const bson_t *bsonMetadata, const char *nameSpace);\
ADIOS2_FOREACH_STDTYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation
} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif /* ADIOS2_ENGINE_JULEAINTERACTION_H_ */
