#!/usr/bin/ruby

DIFF = "diff"

LIST_TEST = 
[
  "test_01",
  "test_01b",
  "test_02",
  "test_03",
  "test_04"
]


def launch_test(list_test)
  
  list_test.each do |test|
    r = `#{EXE_JINJA} -i #{test}.jinja -o #{test}.result`
    process_status = $?
    puts("#{EXE_JINJA} -i #{test}.jinja -o #{test}.result")
    if (process_status.exitstatus != 0)
      puts("launch test failed for #{test} result = #{r.to_i}")
      exit(-1)
    end
    
    if (!File.exist?("#{test}.result") || !File.exist?("#{test}.expected"))
       puts("File result or expected doesn't exist for #{test}")
       exit(-1)
    end
       
    r = `#{DIFF} #{test}.result #{test}.expected`
    process_status = $?
    #puts("#{DIFF} #{test}.result #{test}.expected")
    #p $?.class
    if (process_status.exitstatus > 0)
      puts("#{test} failed !")
      puts("diff result:\n#{r}")
      exit(-1)
    else
      puts("#{test} ok !")
    end
  end
end

EXE_JINJA=ARGV[0]
Dir.chdir(ARGV[1])
launch_test(LIST_TEST)
exit(0)
