include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/include)

set(INDEXER_SOURCE_FILES 
    main_indexer.cpp
    file_manager.cpp
    parser.cpp
    tokenizer.cpp
    util.cpp)

set(SEARCHER_SOURCE_FILES 
    main_searcher.cpp
    file_manager.cpp
    parser.cpp
    tokenizer.cpp
    util.cpp
    query_tree.cpp)

add_executable(indexer ${INDEXER_SOURCE_FILES})
add_executable(searcher ${SEARCHER_SOURCE_FILES})

target_link_libraries(indexer srell)
target_link_libraries(searcher srell)