require_relative './matcher'

unless ARGV.size == 3
  puts "Usage: #{_FILE_} description_file noun_modifier_file output_file"
  exit
end

matcher = Matcher.new(*ARGV)
matcher.parse_and_write