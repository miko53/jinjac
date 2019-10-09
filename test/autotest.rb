#!/usr/bin/ruby

DIFF = "diff"
BUFFER_MODE = ""

def check_memleak(valgrind_result)
  
  founded = false
  nb_error = 0
  
  valgrind_result.each_line do |line|
    if (line =~ /^==\d+== ERROR SUMMARY: (\d+) errors/)
      nb_error = $1
      founded = true
      #p "MATCH ==> #{nb_error}"
      #p line
    end
    #p line
  end
  
  if (founded == false)
    puts("doesn't find the ERROR SUMMARY line of valgrind output")
    exit(-1)
  end
  
  return nb_error.to_i
end


def launch_test(test)
  r = `#{EXE_JINJAC} #{BUFFER_MODE} -i #{test}.jinja -o #{test}.result`
  process_status = $?
  puts("#{EXE_JINJAC} #{BUFFER_MODE} -i #{test}.jinja -o #{test}.result")
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
  end
end

def launch_memcheck(test)

  r = `valgrind --leak-check=full #{EXE_JINJAC} #{BUFFER_MODE} -i #{test}.jinja -o #{test}.result 2>&1`
  process_status = $?
  puts("valgrind --leak-check=full #{EXE_JINJAC} #{BUFFER_MODE} -i #{test}.jinja -o #{test}.result 2>&1")
  if (process_status.exitstatus != 0)
    puts("launch test failed for #{test} result = #{r.to_i}")
    exit(-1)
  else
    check_memleak(r)
  end
end  

EXE_JINJAC=ARGV[0]

if (ARGV[2].to_i == 0)
  launch_test(ARGV[1])
else
  nb_error = launch_memcheck(ARGV[1])
  if (nb_error != 0)
    puts("Test failed --> Leak memory")
    exit(-1)
  end
end

puts("#{ARGV[1]} ok !")
exit(0)
