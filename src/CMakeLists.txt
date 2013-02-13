set(SRC_LIST crawl_simulation.cc
             dataset.cc
             genetic.cc
             scorer.cc
             timer.cc
             url.cc
)

set(HEADER_LIST crawl_simulation.h
                dataset.h
                evaluation.h
                functions.h
                genetic.h
                gp_selector.h
                scorer.h
                terminals.h
                timer.h
                url.h
)

#aux_source_directory(. SRC_LIST)

add_executable(crawling crawling.cc ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(crawling gp m boost_system boost_thread pthread)

add_executable(baselines baselines.cc ${SRC_LIST} ${HEADER_LIST})
target_link_libraries(baselines gp m boost_system boost_thread pthread)
