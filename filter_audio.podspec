Pod::Spec.new do |s|
  s.name = 'filter_audio'
  s.version = '0.1'
  s.license = 'LICENSE'
  s.summary = 'WebRTC audio processing library'
  s.homepage = 'https://www.pylon.com'
  s.authors = { 'Spokestack' => 'support@pylon.com' }
  s.source = { :git => 'https://github.com/pylon/filter_audio.git', :tag => s.version.to_s }
  s.license = {:type => 'Apache', :file => 'LICENSE'}
  s.ios.deployment_target = '11.0'
  s.swift_version = '4.2'
  s.exclude_files = 'test/*'
  s.source_files = '**/*.{h,c}'
  s.preserve_paths = 'module.modulemap'
  s.public_header_files = 'vad/vad_core.h', 'vad/include/webrtc_vad.h'
  s.pod_target_xcconfig = {'HEADER_SEARCH_PATHS' => '${SRCROOT}/**'}
  s.static_framework = true

end
