cmake_minimum_required(VERSION 3.13)

include(imagesannotator-dataexporters-sanitizers-regular)

set(IADE_EXTRA_COMPILE_OPTIONS 
  ${IADE_EXTRA_COMPILE_OPTIONS}
  -fsanitize=pointer-compare
  -fsanitize=leak 
  -fsanitize=address
)

set(
  IADE_EXTRA_LINK_OPTIONS 
  ${IADE_EXTRA_LINK_OPTIONS} 
  -fsanitize=address
)