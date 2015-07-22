{
  'includes': [
    '../qt/qmapboxgl-app.gypi',
    '../linux/mapboxgl-app.gypi',
  ],

  'conditions': [
    ['test', { 'includes': [ '../test/test.gypi' ] } ],
    ['render', { 'includes': [ '../bin/render.gypi' ] } ],
  ],
}
