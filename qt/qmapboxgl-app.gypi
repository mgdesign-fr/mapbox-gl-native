{
  'includes': [
    '../gyp/common.gypi',
  ],
  'targets': [
    { 'target_name': 'qtapp',
      'product_name': 'qmapbox-gl',
      'type': 'executable',

      'dependencies': [
        '../mbgl.gyp:core',
        '../mbgl.gyp:platform-<(platform_lib)',
        '../mbgl.gyp:http-<(http_lib)',
        '../mbgl.gyp:asset-<(asset_lib)',
        '../mbgl.gyp:cache-<(cache_lib)',
        '../mbgl.gyp:copy_styles',
        '../mbgl.gyp:copy_certificate_bundle',
      ],

      'sources': [
        'main.cpp',
        'mapwindow.cpp',
        'mapwindow.hpp',
        '../include/mbgl/platform/qt/qmapboxgl.hpp',
        '../platform/default/default_styles.cpp',
        '../platform/default/log_stderr.cpp',
        '../platform/qt/qfilesource_p.cpp',
        '../platform/qt/qfilesource_p.hpp',
        '../platform/qt/qmapboxgl.cpp',
        '../platform/qt/qmapboxgl_p.hpp',
      ],

      'include_dirs': [
        '../include',
        '../src',
      ],

      'conditions': [
        ['OS == "linux"', {
          'cflags_cc': [
            '<@(opengl_cflags)',
            '<@(qt_cflags)',
            '-Wno-error'
          ],

          'libraries': [
            '<@(opengl_ldflags)',
            '<@(qt_ldflags)'
          ],
        }],
        ['OS == "mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS': [
              '<@(qt_cflags)',
              '-Wno-error'
            ],
            'OTHER_LDFLAGS': [
              '<@(qt_ldflags)'
            ],
          }
        }],
      ],

      'rules': [
        {
          'rule_name': 'MOC files',
          'extension': 'hpp',
          'process_outputs_as_sources': 1,
          'outputs': [ '<(SHARED_INTERMEDIATE_DIR)/<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cpp' ],
          'action': [ '<(qt_moc)', '<(RULE_INPUT_PATH)', '-o', '<(SHARED_INTERMEDIATE_DIR)/<(RULE_INPUT_DIRNAME)/moc_<(RULE_INPUT_ROOT).cpp' ],
          'message': 'Generating MOC <(RULE_INPUT_ROOT).cpp',
        },
      ],
    },
  ],
}
