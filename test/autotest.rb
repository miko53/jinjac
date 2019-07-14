#!/usr/bin/ruby


EXE_JINJA="../jinja_expression"
DIFF = "diff"

LIST_TEST = 
[
  "test_01"
]


def launch_test(list_test)
  
  list_test.each do |test|
    r = `#{EXE_JINJA} -i #{test}.jinja -o #{test}.result`
    #puts("#{EXE_JINJA} -i #{test}.jinja -o #{test}.result")
    if (r.to_i != 0)
      puts("launch test failed for #{test} result = #{r.to_i}")
      exit(-1)
    end
    
    r = `#{DIFF} #{test}.result #{test}.expected`
    #puts("#{DIFF} #{test}.result #{test}.expected")
    if (r.to_i != 0)
      puts("#{test} failed !")
      puts("diff result:\n#{r}")
      exit(-1)
    else
      puts("#{test} ok !")
    end
  end
end


launch_test(LIST_TEST)
exit(0)
