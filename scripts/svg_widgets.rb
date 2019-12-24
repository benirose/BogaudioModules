#!/usr/bin/ruby

INKSCAPE = '/Applications/Inkscape.app/Contents/Resources/bin/inkscape'
OUTPUT_DECIMAL_PLACES=2

hpp_template = <<HPP_TEMPLATE
/* For %PLUGIN%.cpp:
#include "%MODULE%.hpp"
p->addModel(model%MODULE%);
*/

#pragma once

#include "%HEADER%.hpp"

extern Model* model%MODULE%;

namespace %HEADER% {

struct %MODULE% : BGModule {
%ENUMS%

	%MODULE%() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
%PARAMCONFIGS%
	}

	void reset() override;
	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace %HEADER%
HPP_TEMPLATE

cpp_template = <<CPP_TEMPLATE

#include "%MODULE%.hpp"

void %MODULE%::reset() {
}

void %MODULE%::sampleRateChange() {
	// float sampleRate = APP->engine->getSampleRate();
}

void %MODULE%::processChannel(const ProcessArgs& args, int c) {
}

struct %MODULE%Widget : ModuleWidget {
	static constexpr int hp = %HP%;

	%MODULE%Widget(%MODULE%* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SvgPanel *panel = new SvgPanel();
			panel->box.size = box.size;
			panel->setBackground(APP->window->loadSvg(asset::plugin(pluginInstance, "res/%MODULE%.svg")));
			addChild(panel);
		}

%SCREWS%

%POSITIONS%

%CREATES%
	}
};

Model* model%MODULE% = createModel<%MODULE%, %MODULE%Widget>("%MANUFACTURER%-%MODULE%", "%MODULE_CAPS%", "");
CPP_TEMPLATE

require 'optparse'
options = {
  output: 'list',
  variable_style: 'positions',
  module: 'MODULE',
  plugin: 'PLUGIN',
  manufacturer: 'MANUFACTURER',
  hp: '10',
  param_class: 'RoundBlackKnob',
  input_class: 'Port24',
  output_class: 'Port24',
  light_class: 'SmallLight<GreenLight>',
  comments: false,
  sort: nil
}
option_parser = OptionParser.new do |opts|
  opts.banner = "Usage: #{$0} [options] <svg file>"
  opts.on('--list', 'Output list of widget IDs, positions and dimensions (default output)') do
    options[:output] = 'list'
  end
  opts.on('--ids', 'Output list of widget IDs only') do
    options[:output] = 'ids'
  end
  opts.on('--variables=[STYLE]', %w(positions accessors parameters members initializers), "Output variable declarations for each widget (default style: #{options[:variable_style]})") do |v|
    options[:output] = 'variables'
    options[:variable_style] = v if v
  end
  opts.on('--creates', 'Output ParamcreateWidget, etc, lines for each widget') do
    options[:output] = 'creates'
  end
  opts.on('--enums', 'Output param/input/output/light ID enums') do
    options[:output] = 'enums'
  end
  opts.on('--stub-hpp', 'Output a module class stub header (.hpp)') do
    options[:output] = 'hpp'
  end
  opts.on('--stub-cpp', 'Output a module class stub implementation (.cpp)') do
    options[:output] = 'cpp'
  end
  opts.on('--module=MODULE', "Name of the module class (with --creates, --stub-*; default: #{options[:module]})") do |v|
    options[:module] = v
  end
  opts.on('--plugin=PLUGIN', "Name of the plugin (with --stub-*; default:  #{options[:plugin]})") do |v|
    options[:plugin] = v
  end
  opts.on('--manufacturer=MANUFACTURER', "Name of the manufacturer (with ---stub-*; default:  #{options[:manufacturer]})") do |v|
    options[:manufacturer] = v
  end
  opts.on('--hp=HP', "Module width in multiples of RACK_GRID_WIDTH (with --stub-*; default: #{options[:hp]})") do |v|
    options[:hp] = v if v.to_i > 0
  end
  opts.on('--param-class=CLASS', "Widget type for params (with --creates, --stub-*; default: #{options[:param_class]})") do |v|
    options[:param_class] = v
  end
  opts.on('--input-class=CLASS', "Widget type for inputs (with --creates, --stub-*; default: #{options[:input_class]})") do |v|
    options[:input_class] = v
  end
  opts.on('--output-class=CLASS', "Widget type for outputs (with --creates, --stub-*; default: #{options[:output_class]})") do |v|
    options[:output_class] = v
  end
  opts.on('--light-class=CLASS', "Widget type for lights (with --creates, --stub-*; default: #{options[:light_class]})") do |v|
    options[:light_class] = v
  end
  opts.on('--comments', 'Output "generated by" comments around code') do
    options[:comments] = true
  end
  opts.on('--sort=SORT', %w(ids position), 'Sort widgets for output; "ids" to sort alphabetically, "position" to sort top-down and left-right') do |v|
    options[:sort] = v
  end
  opts.on_tail('-h', '--help', 'Show this message') do
    puts opts
    exit
  end
end
begin
  option_parser.parse!
rescue => e
  STDERR.puts e.to_s
  STDERR.puts "\n"
  STDERR.puts option_parser.help
  exit 1
end

unless ARGV.size >= 1
  STDERR.puts option_parser.help
  exit 1
end
svg_file = ARGV[0]
unless File.exist?(svg_file)
  STDERR.puts "No such file: #{svg_file}"
  exit 1
end
svg_file = File.absolute_path(svg_file)

lines = `#{INKSCAPE} -z -S #{svg_file}`
# FIXME: check for error.

Widget = Struct.new(:id, :x, :y, :width, :height) do
  def to_s
    "#{id} x=#{x} y=#{y} width=#{width} height=#{height}"
  end
end
widgets_by_type = {}
widget_re = %r{^(\w+_(WIDGET|PARAM|INPUT|OUTPUT|LIGHT)),(\d+(?:\.\d+)?),(\d+(?:\.\d+)?),(\d+(?:\.\d+)?),(\d+(?:\.\d+)?)}
lines.split.each do |line|
  if m = widget_re.match(line)
    widget = Widget.new(
      m[1],
      m[3].to_f.round(OUTPUT_DECIMAL_PLACES),
      m[4].to_f.round(OUTPUT_DECIMAL_PLACES),
      m[5].to_f.round(OUTPUT_DECIMAL_PLACES),
      m[6].to_f.round(OUTPUT_DECIMAL_PLACES)
    )
    (widgets_by_type["#{m[2].downcase}s"] ||= []) << widget
  end
end

if options[:sort]
  %w(widgets params inputs outputs lights).each do |type|
    next unless widgets_by_type.key?(type)
    widgets_by_type[type].sort! do |a, b|
      case options[:sort]
      when 'position'
        a.y <=> b.y || a.x <=> b.x || a.id <=> b.id
      else
        a.id <=> b.id
      end
    end
  end
end

def titleize(s)
  return s unless s =~ /_/
  ss = s.downcase.split(/_+/)
  "#{ss[0]}#{ss[1..-1].map { |s| "#{s[0].upcase}#{s[1..-1]}" }.join('')}"
end

def make_comment(prefix, indent)
  s =
    if prefix
      "// generated by #{File.basename($0)}"
    else
      "// end generated by #{File.basename($0)}"
    end
  s = "\t\t#{s}" if indent
  s
end

def make_variables(widgets_by_type, style, comments, indent)
  i1 = indent ? "\t\t" : ''
  groups = [%w(params Param), %w(inputs Input), %w(outputs Output), %w(lights Light)].map do |type|
    (widgets_by_type[type[0]] || []).map do |w|
      case style
      when 'accessors'
        "#{i1}#{type[0]}[#{w.id}];"
      when 'parameters'
        "#{i1}#{type[1]}& #{titleize(w.id)},"
      when 'members'
        "#{i1}#{type[1]}& _#{titleize(w.id)};"
      when 'initializers'
        s = titleize(w.id)
        "#{i1}, _#{s}(#{s})"
      else
        "#{i1}auto #{titleize(w.id)}Position = Vec(#{w.x}, #{w.y});"
      end
    end
  end
  s = groups.reject(&:empty?).map { |g| g.join("\n") }.join("\n\n")
  s = [make_comment(true, indent), s, make_comment(false, indent)].join("\n") if comments
  s
end

def make_creates(widgets_by_type, comments, indent, options)
  i1 = indent ? "\t\t" : ''
  groups = []
  groups << (widgets_by_type['params'] || []).map do |w|
    "#{i1}addParam(createParam<#{options[:param_class]}>(#{titleize(w.id)}Position, module, #{options[:module]}::#{w.id}));"
  end.join("\n")
  groups << (widgets_by_type['inputs'] || []).map do |w|
    "#{i1}addInput(createInput<#{options[:input_class]}>(#{titleize(w.id)}Position, module, #{options[:module]}::#{w.id}));"
  end.join("\n")
  groups << (widgets_by_type['outputs'] || []).map do |w|
    "#{i1}addOutput(createOutput<#{options[:output_class]}>(#{titleize(w.id)}Position, module, #{options[:module]}::#{w.id}));"
  end.join("\n")
  groups << (widgets_by_type['lights'] || []).map do |w|
    "#{i1}addChild(createLight<#{options[:light_class]}>(#{titleize(w.id)}Position, module, #{options[:module]}::#{w.id}));"
  end.join("\n")
  s = groups.reject(&:empty?).join("\n\n")
  s = [make_comment(true, indent), s, make_comment(false, indent)].join("\n") if comments
  s
end

def make_param_configs(widgets_by_type, comments, indent, options)
  i1 = indent ? "\t\t" : ''
  s = (widgets_by_type['params'] || []).map do |w|
		w.id =~ /^(.*)_PARAM$/
    %(#{i1}configParam(#{w.id}, 0.0f, 1.0f, 0.0f, "#{$1.downcase}");)
  end.join("\n")
	s = [make_comment(true, indent), s, make_comment(false, indent)].join("\n") if comments
  s
end

def make_enums(widgets_by_type, comments, indent)
  i1 = indent ? "\t" : ''
  i2 = indent ? "\t\t" : "\t"
  groups = %w(Params Inputs Outputs Lights).map do |type|
    ids = (widgets_by_type[type.downcase] || []).map(&:id)
    if ids.empty? && type == 'Lights'
      nil
    else
      ids << "NUM_#{type.upcase}"
      "#{i1}enum #{type}Ids {\n#{i2}#{ids.join(",\n#{i2}")}\n#{i1}};"
    end
  end.reject { |s| s.nil? }
  s = groups.join("\n\n")
  s = [make_comment(true, indent), s, make_comment(false, indent)].join("\n") if comments
  s
end

def make_screws(hp, comments, indent)
  i1 = indent ? "\t\t" : ''
  ss = []
  if hp <= 6
    ss << 'addChild(createWidget<ScrewSilver>(Vec(0, 0)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));'
  elsif hp <= 13
    ss << 'addChild(createWidget<ScrewSilver>(Vec(0, 0)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 0)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(0, 365)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(box.size.x - 15, 365)));'
  else
    ss << 'addChild(createWidget<ScrewSilver>(Vec(15, 0)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(15, 365)));'
    ss << 'addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));'
  end
  ss = ss.map { |s| "#{i1}#{s}" }
  s = ss.join("\n")
  s = [make_comment(true, false), s, make_comment(false, false)].join("\n") if comments
  s
end

def make_stub(widgets_by_type, template, options)
  comments = options[:comments]
  s = template
  s.gsub!(/%MODULE%/, options[:module])
  s.gsub!(/%MODULE_CAPS%/, options[:module].upcase)
  s.gsub!(/%PLUGIN%/, options[:plugin])
  s.gsub!(/%HEADER%/, options[:plugin].downcase)
  s.gsub!(/%MANUFACTURER%/, options[:manufacturer])
  s.gsub!(/%HP%/, options[:hp])
  s.gsub!(/%ENUMS%/, make_enums(widgets_by_type, false, true))
  s.gsub!(/%SCREWS%/, make_screws(options[:hp].to_i, false, true))
  if widgets_by_type.empty?
    s.gsub!(/%POSITIONS%/, '')
    s.gsub!(/%CREATES%/, '')
    s.gsub!(/%PARAMCONFIGS%/, '')
  else
    s.gsub!(/%POSITIONS%/, make_variables(widgets_by_type, 'positions', !comments, true))
    s.gsub!(/%CREATES%/, make_creates(widgets_by_type, false, true, options))
    s.gsub!(/%PARAMCONFIGS%/, make_param_configs(widgets_by_type, false, true, options))
  end
  s.sub!(/\s*\}\s*(Model\*.*)\Z/, "\n}\n\n\n\\1")
  if widgets_by_type['lights'].nil?
    s.sub!(/, NUM_LIGHTS\)/, ')')
  end
  s = [make_comment(true, false), s, make_comment(false, false)].join("\n") if comments
  s
end

case options[:output]
when 'ids'
  groups = %w(params inputs outputs lights).map do |type|
    (widgets_by_type[type] || []).map(&:id)
  end
  puts groups.reject(&:empty?).map { |g| g.join("\n") }.join("\n\n")
when 'variables'
  puts make_variables(widgets_by_type, options[:variable_style], options[:comments], false)
when 'creates'
  puts make_creates(widgets_by_type, options[:comments], false, options)
when 'enums'
  puts make_enums(widgets_by_type, options[:comments], false)
when 'hpp'
  puts make_stub(widgets_by_type, hpp_template, options)
when 'cpp'
  puts make_stub(widgets_by_type, cpp_template, options)
else
	puts "Widgets:"
  puts widgets_by_type['widgets']
  puts "\nParams:"
  puts widgets_by_type['params']
  puts "\nInputs:"
  puts widgets_by_type['inputs']
  puts "\nOutputs:"
  puts widgets_by_type['outputs']
  puts "\nLights:"
  puts widgets_by_type['lights']
end
