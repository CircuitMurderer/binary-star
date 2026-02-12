require 'optparse'
require 'fileutils'

options = {
  filename: "data.csv",
  count: 100
}

OptionParser.new do |opts|
  opts.banner = "usage: ruby gen_data.rb [options]"
  opts.on("-f NAME", "--file NAME", "Output filename") { |f| options[:filename] = f }
  opts.on("-n COUNT", "--number COUNT", Integer, "Number of data entries") { |n| options[:count] = n }
end.parse!

dir = File.dirname(options[:filename])

FileUtils.mkdir_p(dir) unless dir == "."

begin
  File.open(options[:filename], "w") do |file|
    file.puts "number,value"
    options[:count].times do |i|
      file.puts "#{i + 1},#{rand.round(6)}" # 32-bit float
    end
  end
  puts "generated #{options[:count]} entries to #{options[:filename]}"
rescue StandardError => e
  puts "error: #{e.message}"
end