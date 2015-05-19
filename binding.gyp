{
  'targets': [
    {
      'target_name': 'nodbc',
      'sources': [
        'src/connection.cpp',
        'src/connection_async.cpp',
        'src/connection_util.cpp',
        'src/nodbc.cpp'
      ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")',
        './vendor/picojson'
      ],
      'dependencies': [
        './vendor/nanodbc/nanodbc.gyp:nanodbc'
      ]
    }
  ]
}