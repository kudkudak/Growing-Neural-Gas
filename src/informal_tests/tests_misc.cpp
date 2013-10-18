void test_named_segments(){


    SHMemoryManager sh("Server0");
    
   

    sh.new_named_segment("SegmentDatabase", 10000*sizeof(double));
    
    REPORT("Created segment");

    

    boost::interprocess::managed_shared_memory * sh2 = sh.get_named_segment("SegmentDatabase");
    

    
    sh.get_named_segment("SegmentDatabase")->construct<std::string>("mystring")("Hello");

    REPORT("Created string object");
}
