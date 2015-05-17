{
  'targets': [
    {
      'target_name': 'nodbc',
      'sources': ['src/nodbc.cpp'],
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