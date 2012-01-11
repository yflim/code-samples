class Match
  attr_accessor :match_type, :match_phrase
  
  def initialize(mt = nil, mp = nil)
    self.match_type = mt
    self.match_phrase = mp
  end
  
  def update(mt, mp)
    match_type = mt 
    match_phrase.concat(mp) 
  end
    
end
