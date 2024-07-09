"swig" \
 \
    -c++ \
    -doxygen \
    -python \
    -Wextra \
    -w-201,-314,-302,-312,-321,-322,-362,-451,-503,-512,-509,-560 \
    -module mupdf \
    -outdir build/PyMuPDF-x86_64-shared-tesseract-bsymbolic-release \
    -o platform/python/mupdfcpp_swig.cpp \
    -includeall \
    -DTOFU_CJK_EXT \
    -I./platform/python/include \
    -Iinclude \
    -Iplatform/c++/include \
    -ignoremissing \
    -DMUPDF_FITZ_HEAP_H \
    platform/python/mupdfcpp_swig.i