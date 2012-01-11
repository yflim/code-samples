# Could use some polishing/refactoring

require_relative File.join('.', 'match')
require 'CSV'

class Matcher
  
  OUTPUT_HEADERS_TO_ADD = ['match_type', 'match_phrase', 'cleaned_description']
  
  def initialize(desc_file, nounmod_file, out_file)
    raise ArgumentError, 
      "File #{desc_file} does not exist" unless File.exists?(desc_file)
    raise ArgumentError, 
      "File #{nounmod_file} does not exist" unless File.exists?(nounmod_file)
    @description_file = desc_file
    @out_file = out_file
    @nounmod_hash = build_nounmod_hash(nounmod_file)
    @partial_match_map = build_partial_match_map
  end
  
  # Assumes input is sorted
  def build_nounmod_hash(nounmod_file)
    reader = IO.new(IO.sysopen(nounmod_file, 'r'), 'r')
    raise ArgumentError if reader.gets.nil?
    nounmod_hash = {}
    while line = reader.gets
      words = line.split(' ')
      add_nounmod_hash_entry(words, nounmod_hash, false)
    end
    reader.close
    return nounmod_hash
  end
  
  def parse_and_write
    reader = CSV.new(IO.new(IO.sysopen(@description_file, 'r'), 'r'),
                     :headers => :first_row, :return_headers => true)
    reader.readline
    hdrs = reader.headers
    unless hdr_idx = hdrs.index { |i| i =~ /description/i }
      raise "Data file has no (case-insensitive) \"description\" field" 
    end
    desc_hdr = hdrs[hdr_idx]
    output_hdrs = hdrs.concat(OUTPUT_HEADERS_TO_ADD)
    writer = CSV.open(@out_file, 'w', :write_headers => true,
                      :headers => output_hdrs)
    while line = reader.readline
      matches = []
      if line[desc_hdr]
        line_parts = partition_description(line[desc_hdr])
        line_parts.each do |p|
          matches.concat(get_nounmod_matches(p))
          break if matches.size > 1
        end
        line['cleaned_description'] = line[desc_hdr].dup
      end
      if matches.empty?
        set_line_mtype_and_phrase(line, '', '')
      elsif matches.size == 1
        set_line_mtype_and_phrase(line, matches[0].match_type, 
                                  matches[0].match_phrase)
        line['cleaned_description'].sub!(line['match_phrase'], '')
        line['cleaned_description'].gsub!(/\s{2,}/, ' ')
        line['cleaned_description'].sub!(/^\s{0,}(\S*)/, '\1')
      else
        set_line_mtype_and_phrase(line, 'multiple', '')
      end
      writer.add_row(line)
    end
    reader.close
    writer.close
  end
    
  private 
  
  # Assumes it will be called in sorted order of the strings the words are taken from
  def add_nounmod_hash_entry(words, hash, preexisting_parent)
    if hash.empty?
      hash[''] = {} if preexisting_parent
    end 
    if hash[words[0]]
      preexisting_parent = true
    else
      preexisting_parent = false
      hash[words[0]] = {}
    end
    if (ln = words.length) > 1
      add_nounmod_hash_entry(words[1..ln-1], hash[words[0]], preexisting_parent)
    end
  end
  
  def build_partial_match_map
    unless @nounmod_hash
      raise 'Cannot build partial_match_map if @nounmod_hash is nil'
    end
    partial_match_map = {}
    @nounmod_hash.each do |okey, oval|
      if okey =~ /-/
        partial_match_map[okey.delete("-")] = okey
      else
        oval.each_key do |ikey|
          partial_match_map["#{okey}#{ikey}"] = okey
        end
      end
    end
    return partial_match_map
  end
  
  def partition_description(description)
    d = /(\d+\/\d+)|\d+/
    u = /[A-Z]{1,3}(\.?)/
    by = /X|BY/
    dims_with_units = /#{d}\s?#{u}\s?#{by}\s?#{d}\s?#{u}/
    dims = /#{d}\s?#{by}\s?#{d}/
    dim = (description =~ /[1-9]-WAY/) ? /#{d} / : /#{d}/
    regexes = [dims_with_units, dims, dim]
    parts = []
    curr_parts = ["", "", description]
    until curr_parts[2].empty?
      regexes.each do |r|
        curr_parts = description.partition(r)
        break unless curr_parts[1].empty?
      end
      parts << curr_parts[0]
      description = curr_parts[2]
    end
    return parts
  end
  
  def get_nounmod_matches(string2match)
    words = string2match.split
    numwords = words.length
    matches = []
    i = 0
    while i < numwords
      if m = match_starting?(words[i..words.length-1])
        matches << m
        i += m.match_phrase.split.size
      else
        i += 1
      end
      break if matches.size > 1
    end
    return matches
  end
  
  def match_starting?(words)
    if @nounmod_hash[words[0]]
      m = Match.new('exact', words[0])
      complete_matching(words[1..words.length-1], @nounmod_hash[words[0]], m, nil)
    elsif words[0] =~ /-/
      joined = words[0].delete("-")
      separated = words[0].split("-")
      if @nounmod_hash[joined]
        nounmod_hash = @nounmod_hash[joined]
      elsif @nounmod_hash[separated[0]]
        if @nounmod_hash[separated[0]][separated[1]]
          nounmod_hash = @nounmod_hash[separated[0]][separated[1]]
        end
      end
      if nounmod_hash
        m = Match.new('partial', words[0])
        complete_matching(words[1..words.length-1], nounmod_hash, m, nil)
      end
    elsif nounmod_word = @partial_match_map[words[0]]
      if nounmod_word =~ /-/
        nounmod_hash = @nounmod_hash[nounmod_word]
      else
        nounmod_hash = 
          @nounmod_hash[nounmod_word][words[0].sub(/#{nounmod_word}/, '')] 
      end
      m = Match.new('partial', words[0])
      complete_matching(words[1..words.length-1], nounmod_hash, m, nil)
    elsif words.size > 1 
      if @nounmod_hash["#{words[0]}#{words[1]}"]
        m = Match.new("partial", "#{words[0]} #{words[1]}")
        complete_matching(words[2..words.length-1], 
                          @nounmod_hash["#{words[0]}#{words[1]}"], m, nil)
      elsif @nounmod_hash["#{words[0]}-#{words[1]}"] 
        m = Match.new("partial", "#{words[0]} #{words[1]}")
        complete_matching(words[2..words.length-1], 
                          @nounmod_hash["#{words[0]}-#{words[1]}"], m, nil)
      else
        return nil
      end
    else
      return nil
    end    
  end
  
  def complete_matching(words, nounmod_hash, m, last_terminal_match)
    if nounmod_hash.empty?
      return m
    elsif words.empty?
      last_terminal_match = m if nounmod_hash['']
      return last_terminal_match
    end
    if nounmod_hash[words[0]]
      m.match_phrase.concat(" #{words[0]}")
      nounmod_hash = nounmod_hash[words[0]]
      start = 1
    elsif words[0] =~ /-/
      joined = words[0].delete('-')
      separated = words[0].split('-')
      if nounmod_hash[joined]
        nounmod_hash = nounmod_hash[joined]
      elsif nounmod_hash[separated[0]]
        if nounmod_hash[separated[0]][separated[1]]
          nounmod_hash = nounmod_hash[separated[0]][separated[1]]
        end
      else
        last_terminal_match = m if nounmod_hash['']
        return last_terminal_match
      end
      m.update('partial', " #{words[0]}")
      start = 1
    elsif words.size > 1 
      if nounmod_hash["#{words[0]}#{words[1]}"]
        m.update('partial', " #{words[0]} #{words[1]}")
        nounmod_hash = nounmod_hash["#{words[0]}#{words[1]}"]
        start = 2
      elsif nounmod_hash["#{words[0]}-#{words[1]}"] 
        m.update('partial', " #{words[0]} #{words[1]}")
        nounmod_hash = nounmod_hash["#{words[0]}-#{words[1]}"]
        start = 2
      else
        last_terminal_match = m if nounmod_hash['']
        return last_terminal_match        
      end
    else
      nounmod_hash.each do |nm_word, nm_word_hash|
        if nm_word =~ /-/
          nm_word_parts = nm_word.split("-")
          has_match = true
          nm_word_parts.each do |i| 
            has_match = false unless words[0].include?(i) 
          end
          if has_match
            m.update('partial', " #{words[0]}")
            nounmod_hash = nounmod_hash[nm_word]
            last_terminal_match = m if nounmod_hash[""]
            return complete_matching(words[1..words.length-1], 
                                     nounmod_hash, m, last_terminal_match)
          end
        else
          if words[0].include?(nm_word)
            nm_word_hash.each_key do |nm_word2|
              if words[0].include?(nm_word2)
                m.update('partial', " #{words[0]}")
                nounmod_hash = nounmod_hash[nm_word][nm_word2]
                last_terminal_match = m if nounmod_hash[""]
                return complete_matching(words[1..words.length-1], 
                                         nounmod_hash, m, last_terminal_match)
              end
            end
          end
        end
      end
      last_terminal_match = m if nounmod_hash['']
      return last_terminal_match
    end 
    last_terminal_match = m if nounmod_hash['']
    return complete_matching(words[start..words.length-1], nounmod_hash, 
                             m, last_terminal_match)       
  end
       
  def set_line_mtype_and_phrase(line, mt, mp)
    line['match_type'] = mt
    line['match_phrase'] = mp
  end
         
end

