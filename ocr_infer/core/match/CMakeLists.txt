add_library(
  match
  matcher_engine.cc
  ac/ac_matcher.cc
  fuzzy/fuzzy_matcher.cc
)

target_link_libraries(
  match
  file_handle
  -lglog
)
